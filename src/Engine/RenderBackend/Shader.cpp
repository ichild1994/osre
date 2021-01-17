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
#include <osre/RenderBackend/Shader.h>
#include <osre/IO/Uri.h>
#include <osre/IO/IOService.h>

#include <vector>

namespace OSRE {
namespace RenderBackend {

using namespace OSRE::IO;
using namespace OSRE::Common;

std::vector<std::string> split_string(const std::string &str,
        const std::string &delimiter) {
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos) {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

static const c8 osre_include = "osre_include";

static std::string getIncludeFile( const std::string &current_line ) {
    std::string file;
    if (current_line.empty()) {
        return file;
    }

    std::string::size_type pos = current_line.find(' ');
    if (pos == std::string::npos) {
        return file;
    }

    file = current_line.substr(pos + 1, current_line.size() - pos - 1);
    
    return file;
}

std::string load_source(Stream *stream) {
    size_t size = stream->getSize();
    CPPCore::TArray<c8> buffer;
    buffer.resize(size);
    size_t readSize = stream->read(&buffer[0], size);
    String src(&buffer[0]);

    return src;
}

void join_src( std::string &source, const std::string &include_source ) {
    std::vector<std::string> src = split_string(include_source, "\n");
    for (size_t i = 0; i < src.size(); ++i) {
        source += src[i];
    }
}

class ShaderPreprocessor {
public:
    ShaderPreprocessor() :
            mSource() {
        // empty
    }

    ~ShaderPreprocessor() {
        // empty
    }

    bool preprocess( const IO::Uri &file ) {
        mSource.clear();
        if (!file.isValid()) {
            return false;
        }

        Stream *stream = IOService::getInstance()->openStream(file, Stream::AccessMode::ReadAccess);
        if (nullptr == stream) {
            return false;
        }

        std::string src = load_source(stream);
        std::string::size_type pos = std::string::npos;
        std::vector<std::string> lines = split_string(src, "\n");
        for (size_t i = 0; i < lines.size(); ++i) {
            const std::string &current_line = lines[i];
            pos = current_line.find(osre_include);
            if (pos == std::string::npos) {
                mSource += current_line;
                continue;
            }

            std::string include_file = getIncludeFile(current_line);
            IO::Uri include_uri(include_file);
            Stream *include_stream = IOService::getInstance()->openStream(include_uri, Stream::AccessMode::ReadAccess);
            if (nullptr == include_stream) {
                continue;
            }


            std::string include = load_source(include_stream);
            join_src(mSource, include);
        }

        return true;
    }
    
    const String &getSource() const {
        return mSource;
    }

private:
    String mSource;
};

Shader::Shader(const String &name) :
        TResource<Shader, ShaderLoader>(name, Uri()),
        m_parameters(),
        m_attributes() {
    ::memset(m_compileState, 0, sizeof(CompileState)*MaxCompileState);
}

Shader::~Shader() {
    // empty
}

void Shader::setSource(ShaderType type, const String &src) {
    if (src != m_src[(ui32)type]) {
        m_src[(ui32)type] = src;
        m_compileState[(ui32)type] = Updated;
    }
}

ResourceState Shader::onLoad( const Uri &uri, ShaderLoader &loader ) {
    if (getState() == ResourceState::Loaded) {
        return ResourceState::Loaded;
    }

    return getState();
}

Common::ResourceState Shader::onUnload( ShaderLoader &loader ) {
    if (getState() == ResourceState::Unloaded) {
        return ResourceState::Unloaded;
    }

    return getState();
}

} // Namespace RenderBackend
} // Namespace OSRE
