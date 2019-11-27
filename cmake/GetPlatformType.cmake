

if(NOT "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" AND NOT "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
	message(FATAL_ERROR "CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P}. Unknown platform")
else()
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
		add_compile_definitions(__PLAT_TYPE_32__)
		set(PLATFORM_TYPE 32)
		if(WIN32)
			set(PLATFOTM_NAME win32)	
		endif()
	else()
		add_compile_definitions(__PLAT_TYPE_64__)
		set(PLATFORM_TYPE 64)
		if(WIN32)
			set(PLATFOTM_NAME x64)	
		endif()
	endif()
endif()
