# Project Name
TARGET = Supersaw

USE_DAISYSP_LGPL = 1

# Options
CPP_SOURCES = Supersaw.cpp
CPP_STANDARD ?= -std=gnu++17

# https://forum.electro-smith.com/t/out-of-flash-memory-walkthrough-with-samples/4370
OPT = -Os

# Library Locations
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
