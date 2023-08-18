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