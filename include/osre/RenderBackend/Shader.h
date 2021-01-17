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

#include <osre/Common/osre_common.h>
#include <osre/RenderBackend/RenderCommon.h>
#include <cppcore/Container/TArray.h>

namespace OSRE {
namespace RenderBackend {

class Shader;

class ShaderLoader {
public:
    ShaderLoader();
    ~ShaderLoader();
    size_t load(const IO::Uri &uri, Shader *shader);
    bool unload(Texture *tex);
};

class OSRE_EXPORT Shader : public Common::TResource<Shader, ShaderLoader> {
public:
    CPPCore::TArray<String>  m_parameters;
    CPPCore::TArray<String>  m_attributes;
    String                   m_src[MaxShaderTypes];

    Shader(const String &name);
    ~Shader();
    void setSource(ShaderType type, const String &src);
    bool loadSourceFromFile(ShaderType type, const IO::Uri &file);

protected:
    Common::ResourceState onLoad(const IO::Uri &uri, ShaderLoader &loader) override;
    Common::ResourceState onUnload(ShaderLoader &loader) override;

    OSRE_NON_COPYABLE(Shader)

private:
    enum CompileState {
        Updated = 0,
        Compiled,
        Error,
        MaxCompileState
    };

    CompileState m_compileState[MaxCompileState];
};

} // Namespace RenderBackend
} // Namespace OSRE
