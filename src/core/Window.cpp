#include "pgk/core/Window.h"

#include <stdexcept>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace pgk {

namespace {

int s_windowCount = 0;

void glfwErrorCallback(int error, const char* description)
{
    std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

} // namespace

Window::Window(int width, int height, const std::string& title)
    : m_width(width)
    , m_height(height)
{
    if (s_windowCount == 0) {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_handle) {
        if (s_windowCount == 0) {
            glfwTerminate();
        }
        throw std::runtime_error("Failed to create GLFW window");
    }

    ++s_windowCount;

    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(0);

    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        throw std::runtime_error("Failed to load OpenGL function pointers (glad)");
    }

    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);
}

Window::~Window()
{
    if (m_handle) {
        glfwDestroyWindow(m_handle);
        --s_windowCount;
    }

    if (s_windowCount == 0) {
        glfwTerminate();
    }
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_handle) != 0;
}

void Window::setShouldClose(bool value)
{
    glfwSetWindowShouldClose(m_handle, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_handle);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::setResizeCallback(ResizeCallback callback)
{
    m_resizeCallback = std::move(callback);
}

void Window::setTitle(const std::string& title) const
{
    glfwSetWindowTitle(m_handle, title.c_str());
}

void Window::framebufferSizeCallback(GLFWwindow* handle, int width, int height)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if (!self) {
        return;
    }

    self->m_width = width;
    self->m_height = height;

    if (self->m_resizeCallback) {
        self->m_resizeCallback(width, height);
    }
}

} // namespace pgk
