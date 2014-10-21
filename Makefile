# Target specific macros
RPATH=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PATH_TO_ROOT= $(subst .. ,../,$(strip $(patsubst %,..,$(subst /, ,$(RPATH)))))
TARGET = solveAMaze
TARGET_SOURCES = \
	utils.c \
	display.c \
	localization.c \
	mapping.c \
	shared_variables.c \
	movement.c \
	mainController.c \
	$(TARGET).c
TOPPERS_OSEK_OIL_SOURCE = ./$(TARGET).oil

# Don't modify below part
O_PATH ?= build
include $(PATH_TO_ROOT)/nxtOsek/ecrobot/ecrobot.mak
