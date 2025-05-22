cmake_minimum_required(VERSION 3.22)
project(Chip8Emulator VERSION 1.0)
set(TARGET_NAME "Chip8Emulator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

option(PRODUCTION_BUILD "Production build" OFF)

if(PRODUCTION_BUILD)
	set(FullOutputDir "${CMAKE_SOURCE_DIR}/bin/Production")
else()
	set(FullOutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
endif()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	endif()

if(MSVC) 
	add_compile_options(/arch:AVX2)
	add_compile_options(/utf-8)

	if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)
    endif()
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
endif()

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDLTTF_VENDORED ON)

add_subdirectory(thirdparty/SDL-release-3.2.14 EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/SDL_ttf-release-3.2.2 EXCLUDE_FROM_ALL)

add_executable("${TARGET_NAME}" icon.rc)
target_compile_definitions("${TARGET_NAME}" PUBLIC IDI_ICON1=102) 

if(MSVC)
	target_compile_definitions("${TARGET_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	if(PRODUCTION_BUILD)
		set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	endif()
endif()

if(PRODUCTION_BUILD)
	target_compile_definitions("${TARGET_NAME}" PUBLIC DATA_PATH="./data/") 
	target_compile_definitions("${TARGET_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
	target_compile_definitions("${TARGET_NAME}" PUBLIC DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/")
	target_compile_definitions("${TARGET_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources("${TARGET_NAME}" PRIVATE ${MY_SOURCES})

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL_ttf-release-3.2.2/include/")
target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL-release-3.2.14/include/")

target_link_libraries("${TARGET_NAME}" PRIVATE
	SDL3::SDL3-static
	SDL3_ttf::SDL3_ttf
)