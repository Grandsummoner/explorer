RACK_DIR ?= ../..

FLAGS +=
CFLAGS +=
CXXFLAGS += -std=c++17 -I./src
LDFLAGS +=

# Include all source files (excluding Daisy hardware main_daisy.cpp)
SOURCES += $(filter-out src/main_daisy.cpp, $(wildcard src/*.cpp))

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

include $(RACK_DIR)/plugin.mk
