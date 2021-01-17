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
#pragma once

#include <osre/Scene/SceneCommon.h>
#include <osre/RenderBackend/RenderCommon.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace OSRE {
namespace Scene {


//-------------------------------------------------------------------------------------------------
///	@ingroup	Engine
///
///	@brief  This class is used to create the static mesh for base shapes like a triangle or a 
///	text box. 
//-------------------------------------------------------------------------------------------------
class OSRE_EXPORT MeshBuilder {
public:
    /// @brief  The class constructor.
    MeshBuilder();

    /// @brief  The class destructor.
    ~MeshBuilder();

    /// @brief  Will allocate an empty mesh.
    ///	@param  type        [in] The vertex type.
    MeshBuilder &allocEmptyMesh( RenderBackend::VertexType type, ui32 numMeshes );

    /// @brief  Will allocate a triangle mesh.
    ///	@param  type        [in] The vertex type.
    /// @return The created mesh.
    MeshBuilder& allocTriangles( RenderBackend::VertexType type, RenderBackend::BufferAccessType access );

    ///	@brief  Will allocate vertices for a quad primitive.
    ///	@param  type        [in] The vertex type.
    /// @return The created mesh.
    MeshBuilder& allocQuads( RenderBackend::VertexType type, RenderBackend::BufferAccessType access );

    MeshBuilder& allocUiQuad( const Rect2ui &dim, RenderBackend::UiVertexCache &vc, RenderBackend::UiIndexCache &ic );
    
    MeshBuilder& allocCube( RenderBackend::VertexType type, f32 w, f32 h, f32 d, RenderBackend::BufferAccessType access );

    ///	@brief  Will allocate vertices for a list of lines.
    ///	@param  type        [in] The vertex type.
    MeshBuilder& allocLineList( RenderBackend::VertexType type, RenderBackend::BufferAccessType access,
            ui32 numLines, glm::vec3 *posArray, glm::vec3 *colorArray, ui32 *indices );

    MeshBuilder& allocPoints( RenderBackend::VertexType type, RenderBackend::BufferAccessType access,
            ui32 numPoints, glm::vec3 *posArray, glm::vec3 *colorArray );

    ///	@brief  Will allocate vertices for a text-box.
    /// @param  x           [in] Left position of the text box.
    /// @param  y           [in] Upper position of the text box.
    /// @param  textSize    [in] The size for a single glyph.
    /// @param  text        [in] The text to render.
    /// @return The created mesh.
    MeshBuilder& allocTextBox( f32 x, f32 y, f32 textSize, const String &text, RenderBackend::BufferAccessType access );

    /// @brief  Helper method to create text-box mesh.
    static void allocUiTextBox(f32 x, f32 y, i64 stackIndex, f32 textSize, const String &text, RenderBackend::BufferAccessType access,
                RenderBackend::UiVertexCache &vc, RenderBackend::UiIndexCache &ic);

    /// @brief  Will update the vertices of a text box.
    /// @param  geo         [inout] The mesh to update.
    /// @param  textSize    [in] The new text size.
    /// @param  text        [in] The updated text.
    static void updateTextBox( RenderBackend::Mesh *geo, f32 textSize, const String &text );

    ///	@brief  Allocates vertices into a buffer data.
    /// @param  type        [in] The vertex type to create.
    ///	@param  numVerts    [in] The number of vertices to create.
    ///	@param  pos         [in] Pointer to array with vec3-positions, set to nullptr if nothing shall prepared
    ///	@param  col1        [in] Pointer to array with vec3-diffuse colors, set to nullptr if nothing shall prepared
    /// @return The allocated buffer data.
    static RenderBackend::BufferData *allocVertices(RenderBackend::VertexType type, size_t numVerts, ::glm::vec3 *pos,
            ::glm::vec3 *col1, ::glm::vec2 *tex0, RenderBackend::BufferAccessType access );

    static void updateTextVertices( size_t numVerts, ::glm::vec2 *tex0, RenderBackend::BufferData *vb );

    /// @brief  Will return the mesh instance.
    /// @return The mesh instance.
    RenderBackend::Mesh *getMesh();

    /// No copying.
    MeshBuilder( const MeshBuilder& ) = delete;
    MeshBuilder(MeshBuilder&&) = delete;
    MeshBuilder &operator = ( const MeshBuilder& ) = delete;

private:
    CPPCore::TArray<glm::vec3> mPosCache;
    CPPCore::TArray<glm::vec3> mDiffuseColCache;
    CPPCore::TArray<glm::vec3> mNormalCache;
    CPPCore::TArray<glm::vec2> mTex0Cache;
    RenderBackend::PrimitiveGroup *mActivePrimGroup;
    CPPCore::TArray<ui32> mIndexCache;
    CPPCore::TArray<RenderBackend::PrimitiveGroup*> mPrimGroupCache;
    bool mIsDirty;
    RenderBackend::Mesh *mActiveGeo;
};

} // Namespace Scene
} // namespace OSRE
