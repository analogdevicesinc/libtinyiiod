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

cmake_minimum_required(VERSION 2.8.7)
project(tinyiiod C)

set(TINYIIOD_VERSION_MAJOR 0)
set(TINYIIOD_VERSION_MINOR 1)
set(TINYIIOD_VERSION ${TINYIIOD_VERSION_MAJOR}.${TINYIIOD_VERSION_MINOR})

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

add_definitions(-D_USE_STD_INT_TYPES)
add_definitions(-DIIOD_BUFFER_SIZE=0x1000)
add_definitions(-DTINYIIOD_VERSION_MAJOR=${TINYIIOD_VERSION_MAJOR})
add_definitions(-DTINYIIOD_VERSION_MINOR=${TINYIIOD_VERSION_MINOR})
add_definitions(-DTINYIIOD_VERSION_GIT="${TINYIIOD_VERSION_GIT}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(tinyiiod tinyiiod.c parser.c)
set_target_properties(tinyiiod PROPERTIES
	VERSION ${TINYIIOD_VERSION}
	SOVERSION ${TINYIIOD_VERSION_MAJOR}
	PUBLIC_HEADER tinyiiod.h
)

project(tinyiiod-test C)
add_executable(tinyiiod-test example.c)
target_link_libraries(tinyiiod-test tinyiiod)
