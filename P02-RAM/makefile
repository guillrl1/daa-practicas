# Universidad de La Laguna
# Escuela Superior de Ingeniería y Tecnología
# Grado en Ingeniería Informática
# Asignatura: Diseño y Análisis de Algoritmos
# Curso: 3º
# Práctica 2: Simulador de la Máquina RAM
# Autores: Manuel Cadenas García alu0101636849@ull.edu.es
#          Guillermo López Concepción alu0101620459@ull.edu.es
# Fecha: 12/02/2026

# Compiler configuration
CXX = g++
STD = -std=c++20
CPPFLAGS := -Iinclude

# Build modes
DEBUG_FLAGS       = -g -Wall -Wextra -Wpedantic
RELEASE_FLAGS     = -O3

# Default build mode
BUILD ?= debug

ifeq ($(BUILD),debug)
  CXXFLAGS = $(STD) $(DEBUG_FLAGS)
else
  CXXFLAGS = $(STD) $(RELEASE_FLAGS)
endif

# Directorios
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := include

CPPFLAGS := $(shell find $(INCLUDE_DIR) -type d | sed 's/^/-I/')

# Targets y files
TARGET  := $(BIN_DIR)/p01
SOURCES := $(shell find $(SRC_DIR) -name '*.cc')
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Main rules
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Create necessary directories if they don't exist
$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

# Utility rules
.PHONY: all clean debug release

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	@rm -rvf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpieza completada."