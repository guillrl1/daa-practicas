# Universidad de La Laguna
# Escuela Superior de Ingeniería y Tecnología
# Grado en Ingeniería Informática
# Asignatura: Diseño y Análisis de Algoritmos
# Curso: 3º
# Práctica 3 y 4: DyV + Programación Dinámica + Factory
# Autores: Manuel Cadenas García alumno
#          Guillermo López Concepción guillrl1@users.noreply.github.com
# Fecha: 10/03/2026

CXX  = g++
STD  = -std=c++20

OS := $(shell uname -s)

ifeq ($(OS), Darwin)
    MAC_ARCH := $(shell uname -m)
    ifeq ($(MAC_ARCH), arm64)
        GLPK_PREFIX = /opt/homebrew
    else
        GLPK_PREFIX = /usr/local
    endif
    ARCH =
else ifeq ($(OS), Linux)
    GLPK_PREFIX = /usr
    ARCH =
else
    $(error Sistema Operativo no soportado o irreconocible: $(OS))
endif

GLPK_INC = $(GLPK_PREFIX)/include
GLPK_LIB = $(GLPK_PREFIX)/lib
LDFLAGS  = -L$(GLPK_LIB) -lglpk

DEBUG_FLAGS   = -g -Wall -Wextra -Wpedantic
RELEASE_FLAGS = -O3

BUILD ?= debug
ifeq ($(BUILD),debug)
CXXFLAGS = $(STD) $(ARCH) $(DEBUG_FLAGS)
else
CXXFLAGS = $(STD) $(ARCH) $(RELEASE_FLAGS)
endif

SRC_DIR     := src
OBJ_DIR     := obj
BIN_DIR     := bin
LIB_DIR     := lib
INCLUDE_DIR := include

CPPFLAGS := $(shell find $(INCLUDE_DIR) -type d 2>/dev/null | sed 's/^/-I/') \
            -I$(LIB_DIR) \
            -I$(GLPK_INC)

TARGET_P03       := $(BIN_DIR)/p03-dyv
TARGET_BENCHMARK := $(BIN_DIR)/benchmark
TARGET_P04       := $(BIN_DIR)/p04

SHARED_SOURCES := $(filter-out \
                    $(SRC_DIR)/main_p03.cc \
                    $(SRC_DIR)/benchmark_main.cc \
                    $(SRC_DIR)/main_p04.cc, \
                    $(shell find $(SRC_DIR) -name '*.cc'))
SHARED_OBJECTS := $(SHARED_SOURCES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

OBJ_P03       := $(OBJ_DIR)/main_p03.o
OBJ_BENCHMARK := $(OBJ_DIR)/benchmark_main.o
OBJ_P04       := $(OBJ_DIR)/main_p04.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean debug release check-glpk

all: check-glpk $(TARGET_P03) $(TARGET_BENCHMARK) $(TARGET_P04)



$(TARGET_P03): $(OBJ_P03) $(SHARED_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Compilado: $@"

$(TARGET_BENCHMARK): $(OBJ_BENCHMARK) $(SHARED_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Compilado: $@"

$(TARGET_P04): $(OBJ_P04) $(SHARED_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Compilado: $@"

$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	@rm -rvf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpieza completada."