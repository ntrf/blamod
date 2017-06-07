cmake_minimum_required(VERSION 3.1)

project(blamod)

install(
	DIRECTORY cfg/
	DESTINATION blamod/cfg
	PATTERN "config.cfg" EXCLUDE
	PATTERN "userconfig.cfg" EXCLUDE
)

install(
	DIRECTORY maps/
	DESTINATION blamod/maps
	FILES_MATCHING PATTERN "sdk_*.bsp"
)

install(
	DIRECTORY materials media particles resource scripts sound
	DESTINATION blamod
)

install(
	FILES 
		detail.vbsp 
		gameinfo.txt 
		gamestate.txt 
		lights.rad 
		maplist.txt
	DESTINATION blamod
)
