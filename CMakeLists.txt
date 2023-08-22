cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

project(main LANGUAGES CXX C)

find_package(spdlog REQUIRED)

# library
add_library(logger SHARED src/logger/logger.cpp)
target_include_directories(logger PUBLIC include)
target_link_libraries(logger spdlog::spdlog)

# imgui
file(GLOB IMGUI_SRC imgui/*.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/misc/cpp/imgui_stdlib.cpp)
file(GLOB IMGUI_HEADERS imgui/*.h
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.h
        imgui/backends/imgui_impl_opengl3_loader.h
        imgui/misc/cpp/imgui_stdlib.h) 


file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.h)
add_subdirectory(glm)
add_subdirectory(glfw)
add_subdirectory(glad)
set(GLOBAL_DEPENDENCES glm glad glfw logger)

add_executable(text ./src/text.cc ${headers})
target_include_directories(text PUBLIC include)
target_link_libraries(text PUBLIC ${GLOBAL_DEPENDENCES})

# Three body problem
add_executable(tribody ./src/tribody.cpp ${headers})
target_include_directories(tribody PUBLIC include)
target_link_libraries(tribody PUBLIC ${GLOBAL_DEPENDENCES})

add_executable(loadpcd ./src/pcd_load.cc ${headers})
target_include_directories(loadpcd PUBLIC include)
target_link_libraries(loadpcd PUBLIC ${GLOBAL_DEPENDENCES})


add_executable(example_imgui ./src/example_imgui.cpp  ${IMGUI_HEADERS} ${IMGUI_SRC} ${headers})
target_include_directories(example_imgui PUBLIC include imgui imgui/backends imgui/misc/cpp)
target_link_libraries(example_imgui PUBLIC ${GLOBAL_DEPENDENCES})