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
#include <osre/Common/osre_common.h>
#include <osre/RenderBackend/Pipeline.h>

namespace OSRE {
namespace RenderBackend {

namespace Details {
static const c8 *RenderPassNames[] = {
    "RenderPass",
    "UiPass",
    "DbgPass"
};
} // Namespace Details

PipelinePass::PipelinePass(ui32 id, Shader *shader) :
        m_id(id), m_renderTarget(), m_states(), m_shader(shader) {
    // empty
}

PipelinePass::~PipelinePass() {
    // empty
}

void PipelinePass::set(RenderTarget &rt, RenderStates &states) {
    m_renderTarget = rt;
    m_states = states;
}

void PipelinePass::setPolygonState(PolygonState polyState) {
    m_states.m_polygonState = polyState;
}

PolygonState PipelinePass::getPolygonState() const {
    return m_states.m_polygonState;
}

void PipelinePass::setCullState(CullState &cullstate) {
    m_states.m_cullState = cullstate;
}

CullState PipelinePass::getCullState() const {
    return m_states.m_cullState;
}

void PipelinePass::setBlendState(BlendState &blendState) {
    m_states.m_blendState = blendState;
}

const BlendState &PipelinePass::getBlendState() const {
    return m_states.m_blendState;
}

void PipelinePass::setSamplerState(SamplerState &samplerState) {
    m_states.m_samplerState = samplerState;
}

const SamplerState &PipelinePass::getSamplerState() const {
    return m_states.m_samplerState;
}

void PipelinePass::setClearState(ClearState &clearState) {
    m_states.m_clearState = clearState;
}

const ClearState &PipelinePass::getClearState() const {
    return m_states.m_clearState;
}

void PipelinePass::setStencilState(StencilState &stencilState) {
    m_states.m_stencilState = stencilState;
}

const StencilState &PipelinePass::getStencilState() const {
    return m_states.m_stencilState;
}

void PipelinePass::setShader(Shader *shader) {
    m_shader = shader;
}

Shader *PipelinePass::getShader() const {
    return m_shader;
}

ui32 PipelinePass::getId() const {
    return m_id;
}

const c8 *PipelinePass::getPassNameById(ui32 id) {
    if (id >= MaxDbgPasses) {
        return nullptr;
    }

    return Details::RenderPassNames[id];
}

bool PipelinePass::operator==(const PipelinePass &rhs) const {
    return (m_id == rhs.m_id && m_states.m_polygonState == rhs.m_states.m_polygonState && m_states.m_cullState == rhs.m_states.m_cullState && m_states.m_blendState == rhs.m_states.m_blendState && m_states.m_samplerState == rhs.m_states.m_samplerState && m_states.m_clearState == rhs.m_states.m_clearState && m_states.m_stencilState == rhs.m_states.m_stencilState);
}

bool PipelinePass::operator!=(const PipelinePass &rhs) const {
    return !(*this == rhs);
}

Pipeline::Pipeline() :
        m_passes(),
        m_currentPassId(-1),
        m_inFrame(false) {
    // empty
}

Pipeline::~Pipeline() {
    CPPCore::ContainerClear<PipelinePassArray>(m_passes);
}

void Pipeline::addPass(PipelinePass *pass) {
    if (nullptr == pass) {
        return;
    }

    m_passes.add(pass);
}

size_t Pipeline::getNumPasses() const {
    return m_passes.size();
}

size_t Pipeline::beginFrame() {
    if (m_inFrame) {
        return 0L;
    }

    if (m_passes.isEmpty()) {
        return 0L;
    }

    m_inFrame = true;

    return m_passes.size();
}

PipelinePass *Pipeline::beginPass(ui32 passId) {
    if (!m_inFrame) {
        return nullptr;
    }

    m_currentPassId = passId;
    PipelinePass *pass = m_passes[passId];
    if (nullptr == pass) {
        return nullptr;
    }

    return pass;
}

bool Pipeline::endPass(ui32 passId) {
    if (static_cast<i64>(passId) != m_currentPassId || !m_inFrame) {
        return false;
    }

    m_currentPassId = -1;

    return true;
}

void Pipeline::endFrame() {
    m_inFrame = false;
}

void Pipeline::clear() {
    m_currentPassId = -1;
    m_inFrame = false;
    m_passes.resize(0);
}

} // Namespace RenderBackend
} // Namespace OSRE
