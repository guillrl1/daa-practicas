/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 2: Simulador de la Máquina RAM
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 12/02/2026
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <map>

/**
 * @class DataMemory
 * @brief Simulates the data memory of a RAM machine, which consists of an 
 *        unbounded number of registers.
 */
class DataMemory {
public:
  /**
   * @brief Reads a value from the specified register and internal index. 
   *        If the internal index is out of bounds, it will be resized to accommodate it.
   * @param index The register index to read from.
   * @param internal_index The internal index within the register (for vector registers). 
   *                       Defaults to 0 for scalar registers.
   * @return The integer value read from the specified register and internal index.
   * @throws std::out_of_range if the register index is negative.
   */
  int read(int index, size_t internal_index = 0) { 
    if (internal_index >= registers_[index].size()) {
      // Para leer se puede hacer un resize o lanzar una excepción
      registers_[index].resize(internal_index + 1, 0);
    }
    return registers_[index][internal_index]; 
  }
  
  /**
   * @brief Writes a value to the specified register and internal index. If the internal index is out of bounds, it will be resized to accommodate it.
   * @param index The register index to write to.
   * @param value The integer value to write to the specified register and internal index.
   * @param internal_index The internal index within the register (for vector registers). 
   *                       Defaults to 0 for scalar registers.
   * @throws std::out_of_range if the register index is negative.
   */
  void write(int index, int value, size_t internal_index = 0) {
    if (internal_index >= registers_[index].size()) {
      registers_[index].resize(internal_index + 1, 0);
    }
    registers_[index][internal_index] = value;
  }

  /**
   * @brief Returns a const reference to the internal map of registers, where each register index maps to a vector of integers (to support both scalar and vector registers).
   * @return A const reference to the map of registers.
   */
  const std::map<int, std::vector<int>>& getRegisters() const {
    return registers_;
  }
private:
  std::map<int, std::vector<int>> registers_;
};

#endif // MEMORY_H