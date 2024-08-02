# CMakeList.txt : CMake project for EECompiler, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.17)

set(VCPKG_TARGET_TRIPLET x64-windows-static CACHE STRING "VCPKG Target Triplet to use")
set(VCPKG_DEFAULT_TRIPLET x64-windows-static CACHE STRING "VCPKG Target Triplet to use")




project ("EEDevBuilder")


set(CMAKE_CXX_STANDARD 14)



find_package(Freetype REQUIRED)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(soil2 CONFIG REQUIRED)

find_package(unofficial-nuklear CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)




include_directories( "./" )
include_directories( "./EEmbSimulator/" )
include_directories( "./Renderer" )




file(GLOB EEC_SOURCES 
"main.cpp"
"EEmbSimulator/*.cpp"
"EEmbSimulator/*.h"
"Renderer/*.cpp"
"Renderer/*.h"
)



add_executable (EEDevBuilder ${EEC_SOURCES})


  message("BUILD_TYPE: ${BUILD_TYPE}")
  if (BUILD_TYPE AND BUILD_TYPE STREQUAL  "Debug")
  message("DBG is detected")
  set_property(TARGET EEDevBuilder PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  #target_compile_definitions(libeewin64 PRIVATE  EF_IS_SUPER_USER_VER )   
  else()
  message("RELEASE is detected")
  set_property(TARGET EEDevBuilder PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
  endif()


set(RENDER_LIBS nuklear glfw glm::glm GLEW::GLEW soil2 Freetype::Freetype)

target_link_libraries(EEDevBuilder ${RENDER_LIBS} nlohmann_json::nlohmann_json)