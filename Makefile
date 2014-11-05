# Target specific macros
RPATH=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PATH_TO_ROOT= $(subst .. ,../,$(strip $(patsubst %,..,$(subst /, ,$(RPATH)))))
TARGET = solveAMaze
TARGET_SOURCES = \
	utils/utils.c \
	utils/display.c \
	utils/shared_variables.c \
	slam/localization.c \
	slam/mapping.c \
	slam/node.c \
	slam/path_finding.c \
	actions/movement.c \
	actions/commands.c \
	main_controller.c \
	$(TARGET).c
TOPPERS_OSEK_OIL_SOURCE = ./$(TARGET).oil

# Don't modify below part
O_PATH ?= build
include $(PATH_TO_ROOT)/nxtOsek/ecrobot/ecrobot.mak
