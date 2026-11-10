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

#ifndef OUTPUT_TAPE_H
#define OUTPUT_TAPE_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

/**
 * @class OutputTape
 * @brief Simulates the output tape of a RAM machine, allowing values to be 
 *        written and flushed to a file.
 */
class OutputTape {
 public:
  /**
   * @brief Constructs an OutputTape with the specified output file name.
   * @param file_name The name of the file where the output tape will be flushed.
   */
  explicit OutputTape(const std::string& file_name) : file_name_(file_name) {}

  /**
   * @brief Writes a value to the output tape, accumulating it in memory until flush is called.
   * @param value The integer value to be written to the output tape.
   */
  void write(int value) {
    tape_.push_back(value);
  }

  /**
   * @brief Flushes the output tape, writing all accumulated values to the output file.
   *        This is typically called automatically on HALT or on error to ensure that all output is saved.
   * @throws std::runtime_error if the output file cannot be opened for writing.
   */
  void flush() const {
    std::ofstream file(file_name_);
    if (!file.is_open()) {
      throw std::runtime_error("Cannot open output tape file: " + file_name_);
    }
    for (const int value : tape_) {
      file << value << "\n";
    }
  }
 private:
  std::string file_name_;
  std::vector<int> tape_;
};

#endif // OUTPUT_TAPE_H