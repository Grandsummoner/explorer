RACK_DIR ?= ../..

FLAGS +=
CFLAGS +=
CXXFLAGS += -std=c++17 -I./src
LDFLAGS +=

# Strictly specify active VCV Rack module sources (immune to stray/leftover files)
SOURCES := src/plugin.cpp src/Explorer.cpp src/cloud_delay.cpp

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

include $(RACK_DIR)/plugin.mk
