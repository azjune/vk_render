cmake_minimum_required(VERSION 3.3)

## Use the variable PROJECT_NAME for changing the target name
set( PROJECT_NAME "app" )

## Set our project name
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake/modules)
include(external_config)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-D_DEBUG)
endif ()

## Use all the *.cpp files we found under this folder for the project
FILE(GLOB SRCS "*.cpp" "*.c")

## vulkan
find_package(Vulkan)
set(INCLUDES ${INCLUDES} ${VULKAN_INCLUDE_DIR})
set(LIBS ${LIBS} ${VULKAN_LIBRARY})
add_definitions(-DVK_LAYER_PATH=/home/ld016/App/VulkanSDK/1.0.21.1/x86_64/etc/explicit_layer.d)

## glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_DIR})
set(INCLUDES ${INCLUDES} ${GLFW_INCLUDE_DIR})
set(LIBS ${LIBS} glfw)

## glm
set(INCLUDES ${INCLUDES} ${GLM_INCLUDE_DIR})

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    set(LIBS ${LIBS} xcb)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
endif()

include_directories(${INCLUDES})
## Define the executable
add_executable(${PROJECT_NAME} ${SRCS} main.cpp Application.cpp Application.h vk/vk_context.cpp vk/vk_context.h vk/vk_debug.cpp vk/vk_debug.h vk/vk_utils.h vk/vk_utils.cpp)
# target_include_directories(${PROJECT_NAME} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
