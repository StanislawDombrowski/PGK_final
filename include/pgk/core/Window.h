#pragma once

#include <functional>
#include <string>

struct GLFWwindow;

namespace pgk {

class Window {
public:
    using ResizeCallback = std::function<void(int width, int height)>;

    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void setShouldClose(bool value);
    void swapBuffers() const;
    static void pollEvents();

    void setResizeCallback(ResizeCallback callback);
    void setTitle(const std::string& title) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    GLFWwindow* handle() const { return m_handle; }

private:
    static void framebufferSizeCallback(GLFWwindow* handle, int width, int height);

    GLFWwindow* m_handle = nullptr;
    int m_width;
    int m_height;
    ResizeCallback m_resizeCallback;
};

} // namespace pgk
