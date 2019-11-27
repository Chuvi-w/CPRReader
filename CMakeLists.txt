cmake_minimum_required( VERSION 3.10 )
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0074 NEW)
project(CPRReader VERSION 0.0.0.1 DESCRIPTION "CPR Reader")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/")

set(MSVC_USE_STATIC_CRT ON)
include(ChCommonFuncs)
include(GetPlatformType)

SetOutputDirs()

#AddSubdirsRecursive()
AddSubdirsRecursive(${CMAKE_CURRENT_SOURCE_DIR})
GetTargetsRecursive(CurTargets ${CMAKE_CURRENT_SOURCE_DIR})
SetTargetsIDESubdits(CurTargets)