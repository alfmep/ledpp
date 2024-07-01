#
# Copyright (C) 2024 Dan Arrhenius <dan@ultramarin.se>
#
# This file is part of led++.
#
# led++ is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
cmake_minimum_required (VERSION 3.22)


################################################################################
# Project name and info
#
project (led++
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A C++ library and utility application for handling LED lights in Linux."
)


################################################################################
# Options
#
option (BUILD_SHARED_LIBS "Build shared library." OFF)
option (BUILD_DOC "Generate API documentation (if doxygen is found)." ON)
option (BUILD_UTILS "Build led utility application." ON)


include (GenerateExportHeader)
include (CMakePackageConfigHelpers)


################################################################################
# Default build type
#
if (NOT CMAKE_BUILD_TYPE)
    message (STATUS "CMAKE_BUILD_TYPE not specified, use Release")
    set (CMAKE_BUILD_TYPE Release)
endif()


################################################################################
# Common compiler flags
#
set (common_cxx_flags
    -pipe -Wall -D_GNU_SOURCE
)


################################################################################
# Library led++
#
add_library (led++)

set_target_properties (led++ PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER led++.hpp
)

target_compile_options (led++
    PRIVATE
    ${common_cxx_flags}
)
if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
    target_compile_options (led++
        PRIVATE
        -Og -g
    )
endif()

target_include_directories (led++
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INTERFACE
    $<INSTALL_INTERFACE:include>
)

target_sources (led++
    PRIVATE
    led++.cpp
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/led++.hpp>
    $<INSTALL_INTERFACE:include/led++.hpp>
)

target_link_libraries (led++
    INTERFACE
    led++
)

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/led++ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file (
    ${PROJECT_SOURCE_DIR}/led++Config.cmake.in              # The template file
    ${PROJECT_BINARY_DIR}/led++Config.cmake                 # The resulting file
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/led++ # Install destination
##    PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
    # Imported targets do not require the following macros
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)




################################################################################
# Utility application: led
#
if (BUILD_UTILS)
    add_executable (led
        util-led.cpp
    )
    target_compile_options (led++
        PRIVATE
        ${common_cxx_flags}
    )
    if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
        target_compile_options (led++
            PRIVATE
            -Og -g
        )
    endif()
    target_include_directories (led
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )
    target_link_libraries (led
        PRIVATE
        led++
    )
endif()



################################################################################
# Doxygen documentation
#
if (BUILD_DOC)
    find_package (Doxygen)
    if (DOXYGEN_FOUND)
        add_subdirectory (doc)
    endif()
endif()


################################################################################
# Installation
#
include (GNUInstallDirs)


install (TARGETS
    led++
    EXPORT led++Targets
)
install (
    EXPORT led++Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/led++
)
install (
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/led++ConfigVersion.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/led++Config.cmake
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/led++
)


if (BUILD_UTILS)
    install (TARGETS led)
    install (CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
        led \
        ${CMAKE_CURRENT_BINARY_DIR}/lsled \
        )"
    )
    install (
        FILES ${CMAKE_CURRENT_BINARY_DIR}/lsled
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    set_directory_properties (PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/lsled)
endif()


################################################################################
# Un-installation
#
if (NOT TARGET uninstall)
    configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target (uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


################################################################################
# Print summary
#
message (STATUS "Summary:")
if (CMAKE_BUILD_TYPE MATCHES "^$")
    message (STATUS "    Build type........................... [default]")
else()
    message (STATUS "    Build type........................... ${CMAKE_BUILD_TYPE}")
endif()
message (STATUS "    Installation prefix.................. ${CMAKE_INSTALL_PREFIX}")
if (BUILD_SHARED_LIBS)
    message (STATUS "    Build a shared library............... yes")
    message (STATUS "    Build a static library............... no")
else()
    message (STATUS "    Build a shared library............... no")
    message (STATUS "    Build a static library............... yes")
endif()
if (DOXYGEN_FOUND)
    message (STATUS "    Generate API documentation........... yes")
else()
    message (STATUS "    Generate API documentation........... no")
endif()

if (BUILD_UTILS)
    message (STATUS "    Build utilities ..................... yes")
else()
    message (STATUS "    Build utilities ..................... no")
endif()
