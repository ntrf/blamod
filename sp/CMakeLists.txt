cmake_minimum_required(VERSION 3.1)

project(Blamod)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "blamod")
set(CPACK_PACKAGE_VERSION "2.0-alpha9")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "blamod")

include(CPack)

add_subdirectory(src)

add_subdirectory(game/blamod)
