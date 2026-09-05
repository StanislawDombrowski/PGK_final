#pragma once

namespace pgk {

// Averages frame time over ~1-second windows so the reported FPS is stable
// instead of jittering every single frame.
class FpsCounter {
public:
    // Returns true when fps() has just been refreshed (roughly once per
    // second) — callers can use this to know when it's worth pushing a new
    // value somewhere (e.g. the window title) instead of doing so every frame.
    bool update(float deltaSeconds);

    float fps() const { return m_fps; }

private:
    float m_elapsedSeconds = 0.0f;
    int m_frameCount = 0;
    float m_fps = 0.0f;
};

} // namespace pgk
