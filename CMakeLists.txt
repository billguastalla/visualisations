# .
cmake_minimum_required(VERSION 3.11)
project(VISUALISATIONS LANGUAGES CXX VERSION 0.5)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   set(THREADS_PREFER_PTHREAD_FLAG ON)
   find_package(Threads REQUIRED)
endif()

include(FetchContent)

# glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_Populate(glm)

# glfw
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.2
)
FetchContent_GetProperties(glfw)
# from https://jonathanhamberg.com/post/2019-01-27-cmake-dependencies-fetch-content/
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# glad
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)
# from https://jonathanhamberg.com/post/2019-01-27-cmake-dependencies-fetch-content/
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
    set(GLAD_API "gl=3.3" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest")
    set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()

# imgui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.79
)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()
set(SOURCE_LIST_IMGUI 
    "${imgui_SOURCE_DIR}/imgui.h"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imconfig.h"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_internal.h"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imstb_rectpack.h"
    "${imgui_SOURCE_DIR}/imstb_textedit.h"
    "${imgui_SOURCE_DIR}/imstb_truetype.h"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.h"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp"
	)
#add_subdirectory(imgui)



# portaudio
FetchContent_Declare(
  portaudio
  GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
  GIT_TAG        pa_stable_v190600_20161030
)
if(NOT portaudio_POPULATED)
    FetchContent_Populate(portaudio)
    add_subdirectory(${portaudio_SOURCE_DIR} ${portaudio_BINARY_DIR})
endif()

# kissfft
FetchContent_Declare(
  kissfft
  GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
  GIT_TAG        v131
)
if(NOT kissfft_POPULATED)
    FetchContent_Populate(kissfft)
endif()
set(SOURCE_LIST_KISSFFT
    "${kissfft_SOURCE_DIR}/kiss_fft.h"
    "${kissfft_SOURCE_DIR}/kiss_fft.c"
	"${kissfft_SOURCE_DIR}/_kiss_fft_guts.h")

# stbimage
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()


# ffmpeg
# FetchContent_Declare(
  # ffmpeg
  # GIT_REPOSITORY https://git.ffmpeg.org/ffmpeg.git
  # GIT_TAG        n4.3.1
# )
# if(NOT ffmpeg_POPULATED)
    # FetchContent_Populate(ffmpeg)
# endif()


# ffmpeg
set(FFMPEG_BIN "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/bin/")
set(FFMPEG_LIB "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/")
set(FFMPEG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include/")

if(MSVC)
	set(FFMPEG_LIB_AVCODEC    "${FFMPEG_LIB}/avcodec.lib")
	set(FFMPEG_LIB_AVDEVICE   "${FFMPEG_LIB}/avdevice.lib")
	set(FFMPEG_LIB_AVFILTER   "${FFMPEG_LIB}/avfilter.lib")
	set(FFMPEG_LIB_AVFORMAT   "${FFMPEG_LIB}/avformat.lib")
	set(FFMPEG_LIB_AVUTIL     "${FFMPEG_LIB}/avutil.lib")
	set(FFMPEG_LIB_POSTPROC   "${FFMPEG_LIB}/postproc.lib")
	set(FFMPEG_LIB_SWRESAMPLE "${FFMPEG_LIB}/swresample.lib")
	set(FFMPEG_LIB_SWSCALE    "${FFMPEG_LIB}/swscale.lib")
endif()


if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   set(Boost_INCLUDE_DIR "C:/boost/boost_1_74_0")
   set(BOOST_LIBRARYDIR  "C:/boost/boost_1_74_0/stage/lib/")
endif()

find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${BOOST_LIBRARYDIR})
endif()

add_subdirectory(visualisations) 
enable_testing()
add_subdirectory(unit-tests)
