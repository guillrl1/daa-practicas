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

#ifndef JUMP_INSTRUCTION_H
#define JUMP_INSTRUCTION_H

#include "instruction.h"

/**
 * @class JumpInstruction
 * @brief Represents the JUMP instruction in the RAM machine, which unconditionally jumps to the specified label.
 */
class JumpInstruction : public Instruction {
 public:
  explicit JumpInstruction(const std::string& label) : label_{label} {}

  void execute(RAMMachine& machine) override {
    machine.setPC(machine.getLabelAddress(label_));
  }
 private:
  std::string label_;
};

#endif // JUMP_INSTRUCTION_H