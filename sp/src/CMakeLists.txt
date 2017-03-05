cmake_minimum_required(VERSION 3.1)

project(Blamod)

add_subdirectory(tier1)

add_subdirectory(game/client)
add_subdirectory(game/server)

add_custom_target(binaries ALL 
	DEPENDS client server
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../game/blamod_debug"
	
	COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:client>" "./bin/$<TARGET_FILE_NAME:client>"
	COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:server>" "./bin/$<TARGET_FILE_NAME:server>"
)