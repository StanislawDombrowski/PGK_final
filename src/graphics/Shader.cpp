#include "pgk/graphics/Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace pgk {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    const std::string vertexSource = readFile(vertexPath);
    const std::string fragmentSource = readFile(fragmentPath);

    const GLuint vertexShader = compileStage(GL_VERTEX_SHADER, vertexSource, vertexPath);
    const GLuint fragmentShader = compileStage(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    GLint success = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(static_cast<size_t>(logLength));
        glGetProgramInfoLog(m_id, logLength, nullptr, log.data());

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_id);

        throw std::runtime_error(
            "Failed to link shader program (" + vertexPath + ", " + fragmentPath + "): " + log.data());
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::use() const
{
    glUseProgram(m_id);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(uniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(uniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(uniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(uniformLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(uniformLocation(name), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(uniformLocation(name), 1, &value[0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(uniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

std::string Shader::readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint Shader::compileStage(GLenum stage, const std::string& source, const std::string& debugName)
{
    const GLuint shader = glCreateShader(stage);
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(static_cast<size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());

        glDeleteShader(shader);

        throw std::runtime_error("Failed to compile shader (" + debugName + "): " + log.data());
    }

    return shader;
}

GLint Shader::uniformLocation(const std::string& name) const
{
    return glGetUniformLocation(m_id, name.c_str());
}

} // namespace pgk
