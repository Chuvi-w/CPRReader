cmake_minimum_required( VERSION 3.10 )


add_executable( CPRReader 
							CPRReader.cpp 
							CPRReader.h 
							FSHeader.h
							MemFile.cpp
							MemFile.h
							CPRRiff.cpp
							CPRRiff.h
							ScanDir.cpp
							ScanDir.h
							)
target_include_directories(CPRReader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})