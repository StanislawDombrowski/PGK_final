cmake_minimum_required(VERSION 3.20)

project(PGK LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# --- GLFW (windowing + input) ---
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)

# --- GLM (math) ---
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)

# --- glad (OpenGL 3.3 core loader, generated at configure time) ---
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glfw glm glad)

glad_add_library(glad_gl_core_33 STATIC API gl:core=3.3)

file(GLOB_RECURSE PGK_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE PGK_HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(PGK ${PGK_SOURCES} ${PGK_HEADERS})

target_include_directories(PGK PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(PGK PRIVATE glfw glad_gl_core_33 glm::glm)

target_compile_definitions(PGK PRIVATE GLFW_INCLUDE_NONE)

if(MSVC)
    target_compile_options(PGK PRIVATE /W4)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PGK)
endif()

# Keep runtime assets (shaders, textures) next to the built executable.
add_custom_command(TARGET PGK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:PGK>/assets
)
