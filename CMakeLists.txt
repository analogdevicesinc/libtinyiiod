# libtinyiiod - Tiny IIO Daemon Library
#
# Copyright (C) 2016 Analog Devices, Inc.
# Author: Paul Cercueil <paul.cercueil@analog.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

cmake_minimum_required(VERSION 3.1.3)
project(tinyiiod C)

set(TINYIIOD_VERSION_MAJOR 0)
set(TINYIIOD_VERSION_MINOR 1)
set(TINYIIOD_VERSION ${TINYIIOD_VERSION_MAJOR}.${TINYIIOD_VERSION_MINOR})

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_EXAMPLES "Build examples" ON)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
if (WIN32)
	set(CMAKE_SHARED_MODULE_PREFIX "")
	set(CMAKE_STATIC_LIBRARY_PREFIX "")
	SET(CMAKE_IMPORT_LIBRARY_PREFIX "")
endif()

# Get the GIT hash of the latest commit
include(FindGit OPTIONAL)
if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE TINYIIOD_GIT_REPO
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (${TINYIIOD_GIT_REPO} MATCHES ${CMAKE_CURRENT_SOURCE_DIR})
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE TINYIIOD_VERSION_GIT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	endif()
endif()

if (NOT TINYIIOD_VERSION_GIT)
	set(TINYIIOD_VERSION_GIT v${TINYIIOD_VERSION})
endif()

add_definitions(-DTINYIIOD_VERSION_MAJOR=${TINYIIOD_VERSION_MAJOR})
add_definitions(-DTINYIIOD_VERSION_MINOR=${TINYIIOD_VERSION_MINOR})
add_definitions(-DTINYIIOD_VERSION_GIT="${TINYIIOD_VERSION_GIT}")

configure_file(${CMAKE_SOURCE_DIR}/cmake/tinyiiodConfig.cmake ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake @ONLY)

# Set the default install path to /usr/local
if (NOT WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "default install path" FORCE)
endif()

if (BUILD_SHARED_LIBS)
add_library(${PROJECT_NAME} SHARED
		tinyiiod.c
		parser.c)
else()
	add_library(${PROJECT_NAME}
			tinyiiod.c
			parser.c)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_STD_INT_TYPES)
target_compile_definitions(${PROJECT_NAME} PUBLIC IIOD_BUFFER_SIZE=0x1000)
target_compile_definitions(${PROJECT_NAME} PRIVATE TINYIIOD_EXPORTS)

target_include_directories(${PROJECT_NAME}
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/tinyiiod.h>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/compat.h>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	VERSION ${TINYIIOD_VERSION}
	SOVERSION ${TINYIIOD_VERSION_MAJOR}
)

if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
endif()

if (APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES FRAMEWORK FALSE)
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		FRAMEWORK DESTINATION "${OSX_INSTALL_FRAMEWORKSDIR}"
		)

install(FILES ${CMAKE_SOURCE_DIR}/tinyiiod.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(FILES ${CMAKE_SOURCE_DIR}/compat.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Targets
		FILE ${PROJECT_NAME}Targets.cmake
		NAMESPACE ${PROJECT_NAME}::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
		VERSION ${TINYIIOD_VERSION}
		COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

if (BUILD_EXAMPLES)
	project(tinyiiod-test C)
	add_executable(tinyiiod-test example.c)
	target_link_libraries(tinyiiod-test tinyiiod)
endif()
