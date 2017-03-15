cmake_minimum_required(VERSION 3.1)

project(Blamod)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "blamod-release")
set(CPACK_PACKAGE_VERSION "2.0-alpha7")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY FALSE)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "blamod")

include(CPack)

add_subdirectory(src)

add_subdirectory(game/blamod_debug)
