# test/

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(VISUALISATIONSTESTS "TestSuite.cpp")
target_include_directories(VISUALISATIONSTESTS PUBLIC ${VISUALISATIONS_SOURCE_DIR})
target_include_directories(VISUALISATIONSTESTS PUBLIC ${glm_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   target_link_libraries(VISUALISATIONSTESTS PUBLIC Catch2::Catch2 PUBLIC VISUALISATIONS_LIBRARY PRIVATE Threads::Threads)
else()
   target_link_libraries(VISUALISATIONSTESTS PUBLIC Catch2::Catch2 PUBLIC VISUALISATIONS_LIBRARY)
endif()

include(${catch2_SOURCE_DIR}/contrib/Catch.cmake)
catch_discover_tests(VISUALISATIONSTESTS)
