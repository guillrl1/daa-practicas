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

#ifndef INPUT_TAPE_H
#define INPUT_TAPE_H

#include <vector>
#include <string>
#include <stdexcept>

/**
 * @class InputTape
 * @brief Simulates the input tape of a RAM machine, allowing values to be read sequentially. 
 *        It maintains an internal head to track the current read position
 */
class InputTape {
 public:
  /**
   * @brief Constructs an InputTape with the given vector of integer values.
   * @param values A vector of integers representing the values on the input tape.
   * @throws std::invalid_argument if the input vector is empty.
   */
  explicit InputTape(const std::vector<int>& values) : tape_(values), head_(0) {}

  /**
   * @brief Reads the next value from the input tape. Advances the internal head to the next position.
   * @return The integer value read from the tape.
   * @throws std::runtime_error if the input tape has been exhausted (no more values to read).
   */
  int read() {
    if (head_ >= tape_.size()) {
      throw std::runtime_error("Input tape exhausted: no more values to read. Linea " + std::to_string(head_));
    }
    return tape_[head_++];
  }
 private:
  std::vector<int> tape_;
  size_t head_;
};

#endif // INPUT_TAPE_H