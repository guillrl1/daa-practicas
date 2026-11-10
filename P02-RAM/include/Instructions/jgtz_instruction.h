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

#ifndef JGTZ_INSTRUCTION_H
#define JGTZ_INSTRUCTION_H

#include "instruction.h"

/**
 * @class JgtzInstruction
 * @brief Represents the JGTZ instruction in the RAM machine, which jumps to the specified label if the value in register R0 (the accumulator) is greater than zero.
 */
class JgtzInstruction : public Instruction {
 public:
  explicit JgtzInstruction(const std::string& label) : label_{label} {}

  void execute(RAMMachine& machine) override {
    if (machine.getMemory().read(0) > 0) {
      machine.setPC(machine.getLabelAddress(label_));
    }
  }
 private:
  std::string label_;
};

#endif // JGTZ_INSTRUCTION_H