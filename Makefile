MAKE=make
RM=rm -f
ALMOST_CLEAN=$(shell find sp -regextype posix-egrep -regex ".*(server|client)\.(map|so)$$")

all:
	$(MAKE) -C sp/src/ -f game_episodic_linux32.mak

almost-clean:
	-$(RM) $(ALMOST_CLEAN)

.PHONY: almost-clean
