/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2020 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include <osre/Profiling/PerformanceCounterRegistry.h>
#include <osre/Properties/Settings.h>
#include <osre/RenderBackend/Mesh.h>
#include <osre/RenderBackend/RenderBackendService.h>
#include <osre/RenderBackend/RenderCommon.h>
#include <osre/Scene/DbgRenderer.h>
#include <osre/Threading/SystemTask.h>
#include <osre/UI/Widget.h>

#include "OGLRenderer/OGLRenderEventHandler.h"
#include "VulkanRenderer/VlkRenderEventHandler.h"
// clang-format off
#ifdef OSRE_WINDOWS
#   include <osre/Platform/Windows/MinWindows.h>
#endif
// clang-format on
namespace OSRE {
namespace RenderBackend {

using namespace ::OSRE::Common;
using namespace ::OSRE::Threading;
using namespace ::OSRE::Properties;

static const c8 *Tag = "RenderBackendService";

static const c8 *OGL_API = "opengl";
static const c8 *Vulkan_API = "vulkan";

static i64 hasPass(const c8 *id, const ::CPPCore::TArray<PassData *> &passDataArray) {
    for (ui32 i = 0; i < passDataArray.size(); ++i) {
        if (0 == strncmp(passDataArray[i]->m_id, id, strlen(id))) {
            return i;
        }
    }
    return -1;
}

static i64 hasBatch(const c8 *id, const ::CPPCore::TArray<RenderBatchData *> &batchDataArray) {
    for (ui32 i = 0; i < batchDataArray.size(); ++i) {
        if (0 == strncmp(batchDataArray[i]->m_id, id, strlen(id))) {
            return i;
        }
    }
    return -1;
}

RenderBackendService::RenderBackendService() :
        AbstractService("renderbackend/renderbackendserver"),
        m_renderTaskPtr(),
        m_settings(nullptr),
        m_ownsSettingsConfig(false),
        m_frameCreated(false),
        m_renderFrame(&m_frames[0]),
        m_submitFrame(&m_frames[1]),
        m_screen(nullptr),
        m_dirty(false),
        m_passes(),
        m_currentPass(nullptr),
        m_currentBatch(nullptr) {
    // empty
}

RenderBackendService::~RenderBackendService() {
    if (m_ownsSettingsConfig) {
        delete m_settings;
        m_settings = nullptr;
    }
}

bool RenderBackendService::onOpen() {
    if (nullptr == m_settings) {
        m_settings = new Settings;
        m_ownsSettingsConfig = true;
    }

    // Spawn the thread
    if (!m_renderTaskPtr.isValid()) {
        m_renderTaskPtr.init(SystemTask::create("render_task"));
    }

    // Run the render task
    bool ok = m_renderTaskPtr->start(nullptr);
    if (!ok) {
        osre_error(Tag, "Cannot run render task.");
        return ok;
    }

    // Create render event handler for back-end
    const String api = m_settings->get(Settings::RenderAPI).getString();
    if (api == OGL_API) {
        m_renderTaskPtr->attachEventHandler(new OGLRenderEventHandler);
    } else if (api == Vulkan_API) {
        m_renderTaskPtr->attachEventHandler(new VlkRenderEventHandler);
    } else {
        osre_error(Tag, "Requested render-api unknown: " + api);
        ok = false;
    }

    // Create the debug renderer instance
    if (!Scene::DbgRenderer::create(this)) {
        osre_error(Tag, "Cannot create Debug renderer");
        ok = false;
    }

    return ok;
}

bool RenderBackendService::onClose() {
    if (!m_renderTaskPtr.isValid()) {
        return false;
    }

    if (!Scene::DbgRenderer::destroy()) {
        osre_error(Tag, "Cannot destroy Debug renderer");
    }
    if (m_renderTaskPtr->isRunning()) {
        m_renderTaskPtr->detachEventHandler();
        m_renderTaskPtr->stop();
    }

    return true;
}

bool RenderBackendService::onUpdate() {
    if (!m_renderTaskPtr.isValid()) {
        return false;
    }

    if (!m_frameCreated) {
        initPasses();
        m_frameCreated = true;
    }

    commitNextFrame();

    // Synchronizing event with render back-end
    auto result(m_renderTaskPtr->sendEvent(&OnRenderFrameEvent, nullptr));
    m_renderTaskPtr->awaitUpdate();

    return result;
}

void RenderBackendService::setSettings(const Settings *config, bool moveOwnership) {
    if (m_ownsSettingsConfig && m_settings != nullptr) {
        delete m_settings;
        m_settings = nullptr;
        m_ownsSettingsConfig = false;
    }
    m_settings = config;
    m_ownsSettingsConfig = moveOwnership;
}

const Settings *RenderBackendService::getSettings() const {
    return m_settings;
}

void RenderBackendService::initPasses() {
    if (!m_renderTaskPtr.isValid()) {
        return;
    }

    InitPassesEventData *data = new InitPassesEventData;
    m_submitFrame->init(m_passes);
    data->m_frame = m_submitFrame;

    m_renderTaskPtr->sendEvent(&OnInitPassesEvent, data);
}

void RenderBackendService::commitNextFrame() {
    if (!m_renderTaskPtr.isValid()) {
        return;
    }

    CommitFrameEventData *data = new CommitFrameEventData;
    data->m_frame = m_submitFrame;
    for (ui32 i = 0; i < m_passes.size(); ++i) {
        PassData *currentPass = m_passes[i];
        for (ui32 j = 0; j < currentPass->m_geoBatches.size(); ++j) {
            RenderBatchData *currentBatch = currentPass->m_geoBatches[j];
            if (currentBatch->m_dirtyFlag & RenderBatchData::MatrixBufferDirty) {
                FrameSubmitCmd *cmd = m_submitFrame->enqueue();
                cmd->m_passId = currentPass->m_id;
                cmd->m_batchId = currentBatch->m_id;
                cmd->m_updateFlags |= (ui32) FrameSubmitCmd::UpdateMatrixes;
                cmd->m_size = sizeof(MatrixBuffer);
                cmd->m_data = new c8[cmd->m_size];
                ::memcpy(cmd->m_data, &currentBatch->m_matrixBuffer, cmd->m_size);
            } else if (currentBatch->m_dirtyFlag & RenderBatchData::UniformBufferDirty) {
                UniformBuffer &uniformBuffer = data->m_frame->m_uniforBuffers[i];

                for (ui32 k = 0; k < currentBatch->m_uniforms.size(); ++k) {
                    FrameSubmitCmd *cmd = m_submitFrame->enqueue();
                    cmd->m_passId = currentPass->m_id;
                    cmd->m_batchId = currentBatch->m_id;
                    cmd->m_updateFlags |= (ui32)FrameSubmitCmd::UpdateUniforms;
                    UniformVar *var = currentBatch->m_uniforms[k];
                    if (nullptr == var) {
                        continue;
                    }

                    uniformBuffer.writeVar(var);

                    // todo: replace by uniform buffer.
                    cmd->m_size = var->getSize();
                    cmd->m_data = new c8[cmd->m_size];
                    size_t offset(0);
                    cmd->m_data[offset] = var->m_name.size() > 255 ? 255 : static_cast<c8>(var->m_name.size());
                    offset++;
                    ::memcpy(&cmd->m_data[offset], var->m_name.c_str(), var->m_name.size());
                    offset += var->m_name.size();
                    ::memcpy(&cmd->m_data[offset], var->m_data.getData(), var->m_data.m_size);
                }
            } else if (currentBatch->m_dirtyFlag & RenderBatchData::MeshUpdateDirty) {
                for (ui32 k = 0; k < currentBatch->m_updateMeshArray.size(); ++k) {
                    FrameSubmitCmd *cmd = m_submitFrame->enqueue();
                    cmd->m_passId = currentPass->m_id;
                    cmd->m_batchId = currentBatch->m_id;
                    cmd->m_updateFlags |= (ui32)FrameSubmitCmd::UpdateBuffer;
                    Mesh *currentMesh = currentBatch->m_updateMeshArray[k];
                    cmd->m_meshId = currentMesh->m_id;
                    cmd->m_size = currentMesh->m_vb->getSize();
                    cmd->m_data = new c8[cmd->m_size];
                    ::memcpy(cmd->m_data, currentMesh->m_vb->getData(), cmd->m_size);
                }
            } else if (currentBatch->m_dirtyFlag & RenderBatchData::MeshDirty) {
                for (ui32 k = 0; k < currentBatch->m_meshArray.size(); ++k) {
                    if (currentBatch->m_meshArray[k]->m_isDirty) {
                        MeshEntry *meshEntry = currentBatch->m_meshArray[k];
                        for (ui32 l = 0; l < meshEntry->mMeshArray.size(); ++l) {
                            FrameSubmitCmd *cmd = m_submitFrame->enqueue();
                            Mesh *currentMesh = meshEntry->mMeshArray[l];
                            cmd->m_passId = currentPass->m_id;
                            cmd->m_batchId = currentBatch->m_id;
                            cmd->m_updateFlags |= (ui32)FrameSubmitCmd::AddMeshes;
                            cmd->m_meshId = currentMesh->m_id;
                            cmd->m_size = currentMesh->m_vb->getSize();
                            cmd->m_data = new c8[cmd->m_size];
                            ::memcpy(cmd->m_data, currentMesh->m_vb->getData(), cmd->m_size);
                        }
                    }
                }
            }

            currentBatch->m_dirtyFlag = 0;
        }
    }

    data->m_frame = m_submitFrame;
    std::swap(m_submitFrame, m_renderFrame);

    m_renderTaskPtr->sendEvent(&OnCommitFrameEvent, data);
}

void RenderBackendService::sendEvent(const Event *ev, const EventData *eventData) {
    if (m_renderTaskPtr.isValid()) {
        m_renderTaskPtr->sendEvent(ev, eventData);
    }
}

PassData *RenderBackendService::getPassById(const c8 *id) const {
    if (nullptr == id) {
        return nullptr;
    }

    if (nullptr != m_currentPass) {
        if (0 == ::strncmp(m_currentPass->m_id, id, strlen(id))) {
            return m_currentPass;
        }
    }

    for (ui32 i = 0; i < m_passes.size(); ++i) {
        if (0 == ::strncmp(m_passes[i]->m_id, id, strlen(id))) {
            return m_passes[i];
        }
    }

    return nullptr;
}

PassData *RenderBackendService::beginPass(const c8 *id) {
    if (nullptr != m_currentPass) {
        osre_warn(Tag, "Pass recording already active.");
        return nullptr;
    }

    m_currentPass = getPassById(id);
    if (nullptr == m_currentPass) {
        m_currentPass = new PassData(id, nullptr);
    }
    m_dirty = true;

    return m_currentPass;
}

RenderBatchData *RenderBackendService::beginRenderBatch(const c8 *id) {
    if (nullptr == m_currentPass) {
        osre_warn(Tag, "Pass recording not active.");
        return nullptr;
    }

    m_currentBatch = m_currentPass->getBatchById(id);
    if (nullptr == m_currentBatch) {
        m_currentBatch = new RenderBatchData(id);
    }

    return m_currentBatch;
}

void RenderBackendService::setMatrix(MatrixType type, const glm::mat4 &m) {
    if (nullptr == m_currentBatch) {
        osre_error(Tag, "No active batch.");
        return;
    }

    switch (type) {
        case MatrixType::Model:
            m_currentBatch->m_matrixBuffer.m_model = m;
            m_currentBatch->m_dirtyFlag |= RenderBatchData::MatrixBufferDirty;
            break;
        case MatrixType::View:
            m_currentBatch->m_matrixBuffer.m_view = m;
            m_currentBatch->m_dirtyFlag |= RenderBatchData::MatrixBufferDirty;
            break;
        case MatrixType::Projection:
            m_currentBatch->m_matrixBuffer.m_proj = m;
            m_currentBatch->m_dirtyFlag |= RenderBatchData::MatrixBufferDirty;
            break;
        default:
            break;
    }
}

void RenderBackendService::setMatrix(const String &name, const glm::mat4 &matrix) {
    if (nullptr == m_currentBatch) {
        osre_error(Tag, "No active batch.");
        return;
    }

    UniformVar *var = m_currentBatch->getVarByName(name.c_str());
    if (nullptr == var) {
        var = UniformVar::create(name, ParameterType::PT_Mat4);
        m_currentBatch->m_uniforms.add(var);
    }

    m_currentBatch->m_dirtyFlag |= RenderBatchData::UniformBufferDirty;
    ::memcpy(var->m_data.m_data, glm::value_ptr(matrix), sizeof(glm::mat4));
}

void RenderBackendService::setUniform(UniformVar *var) {
    if (nullptr == var) {
        osre_error(Tag, "No active batch.");
        return;
    }

    if (nullptr != m_currentBatch) {
        m_currentBatch->m_uniforms.add(var);
        m_currentBatch->m_dirtyFlag |= RenderBatchData::UniformBufferDirty;
    }
}

void RenderBackendService::setMatrixArray(const String &name, ui32 numMat, const glm::mat4 *matrixArray) {
    if (nullptr == m_currentBatch) {
        osre_error(Tag, "No active batch.");
        return;
    }

    UniformVar *var = m_currentBatch->getVarByName(name.c_str());
    if (nullptr == var) {
        var = UniformVar::create(name, ParameterType::PT_Mat4Array, numMat);
        m_currentBatch->m_uniforms.add(var);
    }

    ::memcpy(var->m_data.m_data, glm::value_ptr(matrixArray[0]), sizeof(glm::mat4) * numMat);
    m_currentBatch->m_dirtyFlag |= RenderBatchData::UniformBufferDirty;
}

void RenderBackendService::addMesh(Mesh *mesh, ui32 numInstances) {
    if (nullptr == mesh) {
        osre_debug(Tag, "Pointer to geometry is nullptr.");
        return;
    }

    if (nullptr == m_currentBatch) {
        osre_error(Tag, "No active batch.");
        return;
    }

    MeshEntry *entry = new MeshEntry;
    entry->mMeshArray.add(mesh);
    entry->numInstances = numInstances;
    m_currentBatch->m_meshArray.add(entry);
    m_currentBatch->m_dirtyFlag |= RenderBatchData::MeshDirty;
}

void RenderBackendService::addMesh(const CPPCore::TArray<Mesh *> &geoArray, ui32 numInstances) {
    if (nullptr == m_currentBatch) {
        osre_error(Tag, "No active batch.");
        return;
    }

    MeshEntry *entry = new MeshEntry;
    entry->numInstances = numInstances;
    entry->mMeshArray.add(&geoArray[0], geoArray.size());
    m_currentBatch->m_meshArray.add(entry);
    m_currentBatch->m_dirtyFlag |= RenderBatchData::MeshDirty;
}

void RenderBackendService::updateMesh(Mesh *mesh) {
    if (nullptr == m_currentBatch) {
        osre_error(Tag, "No active batch.");
        return;
    }
    m_currentBatch->m_updateMeshArray.add(mesh);
    m_currentBatch->m_dirtyFlag |= RenderBatchData::MeshUpdateDirty;
}

bool RenderBackendService::endRenderBatch() {
    if (nullptr == m_currentBatch) {
        return false;
    }

    if (nullptr == m_currentPass) {
        m_currentPass = new PassData("defaultPass", nullptr);
    }

    if (-1 == hasBatch(m_currentBatch->m_id, m_currentPass->m_geoBatches)) {
        m_currentPass->m_geoBatches.add(m_currentBatch);
    }

    m_currentBatch = nullptr;

    return true;
}

bool RenderBackendService::endPass() {
    if (nullptr == m_currentPass) {
        return false;
    }

    if (-1 == hasPass(m_currentPass->m_id, m_passes)) {
        m_passes.add(m_currentPass);
    }
    m_currentPass = nullptr;

    return true;
}

void RenderBackendService::clearPasses() {
    m_currentPass = nullptr;

    for (ui32 i = 0; i < m_passes.size(); ++i) {
        delete m_passes[i];
    }
    m_passes.clear();
    m_frameCreated = false;
}

void RenderBackendService::attachView() {    
}

void RenderBackendService::resize(ui32 x, ui32 y, ui32 w, ui32 h) {
    if (m_screen != nullptr) {
        m_screen->resize(x, y, w, h);
    }

    if (mBehaviour.ResizeViewport) {
        ResizeEventData *data = new ResizeEventData(x, y, w, h);
        m_renderTaskPtr->sendEvent(&OnResizeEvent, data);
    }

}

void RenderBackendService::focusLost() {
    // todo: implement me
}

} // Namespace RenderBackend
} // Namespace OSRE
