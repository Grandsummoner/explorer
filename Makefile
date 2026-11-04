# VCV Rack v2 Plugin Makefile for Hermetic Modular "Explorer"
# Point RACK_DIR to your local Rack-SDK location

FLAGS +=
CFLAGS +=
CXXFLAGS += -std=c++17 -I./src

# Include all source files (excluding Daisy hardware main_daisy.cpp)
SOURCES += $(filter-out src/main_daisy.cpp, $(wildcard src/*.cpp))

# Distributables (assets and plugin manifest)
DISTRIBUTABLES += res plugin.json

# If RACK_DIR is not set, default to standard Rack-SDK in parent dir
RACK_DIR ?= ../..

# Include the Rack plugin makefile
include $(RACK_DIR)/plugin.mk
