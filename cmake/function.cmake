include(CMakeParseArguments)

# creating executable
function(create_executable)
	cmake_parse_arguments(
		PARSED_ARGS
		""
		"TARGET_NAME"
		"INCLUDE_DIRS;LIBS"
		${ARGN}
	)
	set(TARGET_SRC_FILES "src/*.cpp")
	
	file(GLOB TARGET_SRC ${TARGET_SRC_FILES})
	
	add_executable(${PARSED_ARGS_TARGET_NAME} ${TARGET_SRC})
	
	if (USE_FFTW AND NOT FFTW_DIR) 
		add_dependencies(${PARSED_ARGS_TARGET_NAME} project_fftw)
	endif()
	
	target_include_directories(${PARSED_ARGS_TARGET_NAME} PUBLIC
		${PARSED_ARGS_INCLUDE_DIRS}
		${FFT_INCLUDES}
		${MPI_CXX_INCLUDE_DIRS}
	)
	
	if (WIN32)
		target_link_libraries(${PARSED_ARGS_TARGET_NAME}
			${PARSED_ARGS_LIBS}
			${FFT_LIBRARIES}
			${MPI_CXX_LIBRARIES})
	
	elseif (UNIX)
		target_link_libraries(${PARSED_ARGS_TARGET_NAME}
			${PARSED_ARGS_LIBS}
			-Wl,--start-group ${FFT_LIBRARIES} -Wl,--end-group
			${MPI_CXX_LIBRARIES})
	endif()
endfunction()

# creating library
function(create_library)
	cmake_parse_arguments(
		PARSED_ARGS
		""
		"TARGET_NAME"
		"INCLUDE_DIRS"
		${ARGN}
	)
	file(GLOB TARGET_SRC "src/*.cc" "src/*.cpp" "include/*.h")
	add_library(${PARSED_ARGS_TARGET_NAME} STATIC ${TARGET_SRC})
	target_include_directories(${PARSED_ARGS_TARGET_NAME} PUBLIC
		${PARSED_ARGS_INCLUDE_DIRS}
		${FFT_INCLUDES}
		${MPI_CXX_INCLUDE_DIRS}
	)
endfunction()

