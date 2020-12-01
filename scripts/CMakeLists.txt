# Using this min version for now
cmake_minimum_required(VERSION 3.1)

project(challenge_sets)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_policy(SET CMP0057 NEW)
# Needed for newer challenges
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if(WIN32)
    # Need to use MASM on windows
    enable_language(ASM_MASM)

    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)

    add_compile_options(
        /w
        /W0
        /Z7
        /GS-
        /GR-
        /guard:cf-
    )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MACHINE:X86 /NXCOMPAT:NO /SAFESEH:NO /DYNAMICBASE:NO")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MACHINE:X86")

    # Read in challenges to exclude from build
    if(CLANGCL)
        file(STRINGS "exclude/win_clangcl.txt" cb_exclude)
        add_definitions(-DCLANGCL)
    else()
        file(STRINGS "exclude/win_msvc.txt" cb_exclude)
        add_definitions(-DWIN)
    endif()
else(WIN32)
    # Generic ASM can be enabled on other OSs
    enable_language(ASM)

    if(UNIX AND NOT APPLE)
        set(LINUX TRUE)
        add_definitions(-DLINUX)

        # Read in challenges to exclude from build
        file(STRINGS "exclude/linux.txt" cb_exclude)
    endif()
    if(APPLE)
        add_definitions(-DAPPLE)

        # Read in challenges to exclude from build
        file(STRINGS "exclude/osx.txt" cb_exclude)
    endif()

    # Default flags for everything
    add_compile_options(
        -fno-builtin
        -w
        -g3
        -m32
    )

    # Link everything 32-bit (until we have a 64-bit option)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")

    # Linker options
    # Dynamic by default
    option(BUILD_SHARED_LIBS "" ON)
    if(BUILD_STATIC_LIBS AND LINUX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -Wl,--allow-multiple-definition")
        set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
        set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
    endif()
endif(WIN32)

# Build all libraries
include_directories(include)
add_subdirectory(include)

function(target_link_libraries _target)
    set(_mode "PUBLIC")
    foreach(_arg IN LISTS ARGN)
        if (_arg MATCHES "INTERFACE|PUBLIC|PRIVATE|LINK_PRIVATE|LINK_PUBLIC|LINK_INTERFACE_LIBRARIES")
            set(_mode "${_arg}")
        else()
            if (NOT _arg MATCHES "debug|optimized|general")
                set_property(GLOBAL APPEND PROPERTY GlobalTargetDepends${_target} ${_arg})
            endif()
        endif()
    endforeach()
    _target_link_libraries(${_target} ${ARGN})
endfunction()

function(get_link_dependencies _target _listvar)
    set(_worklist ${${_listvar}})
    if (TARGET ${_target})
        list(APPEND _worklist ${_target})
        get_property(_dependencies GLOBAL PROPERTY GlobalTargetDepends${_target})
        foreach(_dependency IN LISTS _dependencies)
            if (NOT _dependency IN_LIST _worklist)
                get_link_dependencies(${_dependency} _worklist)
            endif()
        endforeach()
        set(${_listvar} "${_worklist}" PARENT_SCOPE)
    endif()
endfunction()


function(build path target)
    message(STATUS "Building ${target}")

    # Gather all sources
    aux_source_directory(${path}/src cb_src)
    aux_source_directory(${path}/lib cb_lib)
    aux_source_directory(${path}/include cb_inc)
    set(cb_all_src ${cb_lib} ${cb_src} ${cb_inc})

    add_executable(${target} ${cb_all_src})
    add_executable(${target}_patched ${cb_all_src})


    target_include_directories(${target} PUBLIC ${path}/lib ${path}/src ${path}/include)
    target_include_directories(${target}_patched PUBLIC ${path}/lib ${path}/src ${path}/include)
    #message(${target})
    set_target_properties(${target} PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                          RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${target}_patched PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                          RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})

    # Add patched defines
    target_compile_definitions(${target}_patched PUBLIC ${patch_defs})

    target_link_libraries(${target} LINK_PUBLIC cgc)
    target_link_libraries(${target}_patched LINK_PUBLIC cgc)
    get_link_dependencies(${target} _deps)
    get_target_property(LIBB_INCLUDES ${target} INCLUDE_DIRECTORIES)

    FILE(REMOVE myfile.txt)   
    FILE(REMOVE src.txt)
    string(REPLACE "cb-multios" "cb-multios-tis" output_dir ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(_dep IN LISTS _deps)
    	get_target_property(_srcs ${_dep} SOURCES)
    	get_target_property(_src_dir ${_dep} SOURCE_DIR)
	foreach(_src IN LISTS _srcs)
        	#message("${_src_dir}/${_src}")
		string(REPLACE "cb-multios" "cb-multios-tis" _src_tmp "${_src}")
		string(REPLACE "cb-multios" "cb-multios-tis" _src_dir_tmp "${_src_dir}") 
		#message("${_src}")
		string(FIND "${_src}" "src" output)
		#message("${output}")
		IF(NOT ${output} EQUAL -1)
			get_filename_component(basename ${_src_tmp} NAME)
			IF(${basename} STREQUAL ${_src_tmp})
				FILE(APPEND ${output_dir}/src.txt "${_src_dir_tmp}/${basename}\t")
			ELSE()
				FILE(APPEND ${output_dir}/src.txt "${_src_tmp}\t")
			ENDIF()
		ENDIF()
		
		
    	endforeach()
    endforeach()
    IF(WIN32)
        # Copy required DLLs post-build
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cgc> ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tiny-AES128-C> ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pov> ${CMAKE_CURRENT_BINARY_DIR}
        )
    ENDIF()
endfunction(build)

function(buildPOV path cb pov)
    # Unique target name, i.e. "Palindrome2_pov_1",
    # executable name kept as "pov_#.pov"
    set(target "${cb}_${pov}")

    # Gather sources
    aux_source_directory("${path}/${pov}" pov_src)
    add_executable(${target} ${pov_src})

    # POVs are allowed to include challenge headers,
    # but must have their own definitions
    target_include_directories(
        ${target} PUBLIC
        "${path}/${pov}"
        "${path}/include"
        "${path}/lib"
        "${path}/src"
        "${PROJECT_SOURCE_DIR}/include/libpov/pov"
    )
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${pov}.pov"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(${target} LINK_PUBLIC cgc pov)
endfunction(buildPOV)

function(buildCB)
    # Generate all the patched #defines for this challenge
    set(patch_defs -DPATCHED)
    if(VULN_COUNT)
        foreach(i RANGE 1 ${VULN_COUNT})
            set(patch_defs ${patch_defs} -DPATCHED_${i})
        endforeach()
    endif()

    # Some challenges have multiple binaries that need to be built
    # Check if these directories exist
    file(GLOB cb_parts "${cb_path}/cb_*")
    if(cb_parts)
        # Iterate through the directories and build each
        set(i 1)
        foreach(cb ${cb_parts})
            build(${cb_path}/cb_${i} ${cb_id}_${i})
            MATH(EXPR i "${i} + 1")
        endforeach()
    else()
        # Build normally if there are no extra directories
        build(${cb_path} ${cb_id})
    endif()

    file(GLOB cb_povs "${cb_path}/pov_*")
    if(cb_povs)
        foreach(path ${cb_povs})
            # Get the pov name, i.e. "pov_1"
            get_filename_component(pov ${path} NAME)
            buildPOV(${cb_path} ${cb_id} ${pov})
        endforeach()
    endif()
endfunction(buildCB)

function(buildSO)
    if(UNIX)
        # Build a shared library object for a specific challenge
        set(LIB_DIR ${cb_path}/lib)
        set(SRC_DIR ${cb_path}/src)
        set(INCLUDE_DIR ${cb_path}/include)

        file(GLOB lib_files ${LIB_DIR}/*)
        file(GLOB src_files ${SRC_DIR}/*)
        set(LIB_SRCS ${lib_files} ${src_files})

        set(LIB_NAME ${AUTHOR_ID}_${SERVICE_ID})
        if(BUILD_STATIC_LIBS)
            set(LIB_CGC "${PROJECT_BINARY_DIR}/include/libcgc.a")
        else()
            if(APPLE)
                set(LIB_CGC "${PROJECT_BINARY_DIR}/include/libcgc.dylib")
            else()
                set(LIB_CGC "${PROJECT_BINARY_DIR}/include/libcgc.so")
            endif()
        endif()

        if(BUILD_STATIC_LIBS)
            message(STATUS "Building static library (${LIB_NAME}.a)")
            add_library(${LIB_NAME} STATIC ${LIB_SRCS})
        else()
            message(STATUS "Building shared library (${LIB_NAME}.so)")
            add_library(${LIB_NAME} SHARED ${LIB_SRCS})
        endif()

        target_include_directories(${LIB_NAME} PUBLIC ${LIB_DIR})
        target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
        target_link_libraries(${LIB_NAME} ${LIB_CGC})
    endif()
endfunction(buildSO)

# More options needed for cbs
if(NOT WIN32)
    add_compile_options(
        -fno-stack-protector
    )
endif()

add_definitions(
    -Derrno=__cgc_errno
    -DFORTIFY_SOURCE=0
)

if(LINUX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z execstack -z norelro")
endif()

file(GLOB challenge_binaries challenges/*)
foreach(cb_path ${challenge_binaries})
    if(IS_DIRECTORY ${cb_path} AND EXISTS ${cb_path}/CMakeLists.txt)
        # Get filename
        get_filename_component(cb_id ${cb_path} NAME)

        # Skip the challenge if it's in the exclude list
        list(FIND cb_exclude ${cb_id} idx)
        if(${idx} EQUAL -1)
            add_subdirectory(${cb_path})
        else()
            message(STATUS "EXCLUDE: ${cb_id}")
        endif()
    endif()
endforeach()
