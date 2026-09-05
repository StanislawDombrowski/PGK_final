#include "pgk/utils/FpsCounter.h"

namespace pgk {

bool FpsCounter::update(float deltaSeconds)
{
    m_elapsedSeconds += deltaSeconds;
    ++m_frameCount;

    if (m_elapsedSeconds >= 1.0f) {
        m_fps = static_cast<float>(m_frameCount) / m_elapsedSeconds;
        m_frameCount = 0;
        m_elapsedSeconds = 0.0f;
        return true;
    }

    return false;
}

} // namespace pgk
