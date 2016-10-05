cmake_minimum_required(VERSION 3.1)

project(blamod)

install(
	DIRECTORY cfg/
	DESTINATION ./cfg
	PATTERN "config.cfg" EXCLUDE
)

install(
	DIRECTORY maps/
	DESTINATION ./maps
	FILES_MATCHING PATTERN "*.bsp"
)

install(
	DIRECTORY materials media particles resource scripts sound
	DESTINATION .
)

install(
	FILES detail.vbsp gameinfo.txt gamestate.txt lights.rad
	DESTINATION .
)
