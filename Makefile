RACK_DIR ?= ../..

FLAGS +=
CFLAGS +=
CXXFLAGS += -std=c++17 -I./src
LDFLAGS +=

# Explicitly exclude any hardware target files if present in the tree
SOURCES += $(filter-out src/main_daisy.cpp, $(wildcard src/*.cpp) $(wildcard src/**/*.cpp))

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

include $(RACK_DIR)/plugin.mk
