# Universidad de La Laguna
# Escuela Superior de Ingeniería y Tecnología
# Grado en Ingeniería Informática
# Asignatura: Diseño y Análisis de Algoritmos
# Curso: 3º
# Práctica 3: Experimentación con Divide y Vencerás
# Autores: Manuel Cadenas García alu0101636849@ull.edu.es
#          Guillermo López Concepción alu0101620459@ull.edu.es
# Fecha: 08/03/2026

# ─── Compilador ───────────────────────────────────────────────────────────────
# -arch x86_64 fuerza compilación x86_64 para coincidir con GLPK de Homebrew Intel
CXX = g++
STD = -std=c++20
ARCH = -arch x86_64

# ─── GLPK (Homebrew Intel en /usr/local) ──────────────────────────────────────
GLPK_PREFIX  = /usr/local
GLPK_INC     = $(GLPK_PREFIX)/include
GLPK_LIB     = $(GLPK_PREFIX)/lib
LDFLAGS      = -L$(GLPK_LIB) -lglpk

# ─── Modos de compilación ─────────────────────────────────────────────────────
DEBUG_FLAGS   = -g -Wall -Wextra -Wpedantic
RELEASE_FLAGS = -O3

BUILD ?= debug
ifeq ($(BUILD),debug)
CXXFLAGS = $(STD) $(ARCH) $(DEBUG_FLAGS)
else
CXXFLAGS = $(STD) $(ARCH) $(RELEASE_FLAGS)
endif

# ─── Directorios ──────────────────────────────────────────────────────────────
SRC_DIR     := src
OBJ_DIR     := obj
BIN_DIR     := bin
LIB_DIR     := lib
INCLUDE_DIR := include

CPPFLAGS := $(shell find $(INCLUDE_DIR) -type d | sed 's/^/-I/') \
            -I$(LIB_DIR) \
            -I$(GLPK_INC)

# ─── Ejecutables ──────────────────────────────────────────────────────────────
TARGET_MAIN      := $(BIN_DIR)/p03-dyv
TARGET_BENCHMARK := $(BIN_DIR)/benchmark

# ─── Fuentes compartidas (todo src/ excepto los mains) ────────────────────────
SHARED_SOURCES := $(filter-out $(SRC_DIR)/main.cc $(SRC_DIR)/benchmark_main.cc, \
                    $(shell find $(SRC_DIR) -name '*.cc'))
SHARED_OBJECTS := $(SHARED_SOURCES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

OBJ_MAIN      := $(OBJ_DIR)/main.o
OBJ_BENCHMARK := $(OBJ_DIR)/benchmark_main.o

# ─── Regla de compilación de objetos ─────────────────────────────────────────
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# ─── Reglas principales ───────────────────────────────────────────────────────
.PHONY: all clean debug release check-glpk

all: check-glpk $(TARGET_MAIN) $(TARGET_BENCHMARK)

# Verificar que GLPK arm64 está disponible antes de compilar
check-glpk:
	@if [ ! -f "$(GLPK_INC)/glpk.h" ]; then \
		echo ""; \
		echo "ERROR: No se encuentra $(GLPK_INC)/glpk.h"; \
		echo "Instala GLPK arm64 con:"; \
		echo "  /opt/homebrew/bin/brew install glpk"; \
		echo ""; \
		exit 1; \
	fi
	@echo "✓ GLPK encontrado en $(GLPK_PREFIX)"

# p03-dyv: no necesita GLPK
$(TARGET_MAIN): $(OBJ_MAIN) $(SHARED_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Compilado: $@"

# benchmark: enlaza con GLPK arm64
$(TARGET_BENCHMARK): $(OBJ_BENCHMARK) $(SHARED_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Compilado: $@"

# ─── Creación de directorios ──────────────────────────────────────────────────
$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

# ─── Atajos de modo ───────────────────────────────────────────────────────────
debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

# ─── Limpieza ─────────────────────────────────────────────────────────────────
clean:
	@rm -rvf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpieza completada."