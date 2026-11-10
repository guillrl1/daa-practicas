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

#ifndef LOAD_INSTRUCTION_H
#define LOAD_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

/**
 * @class LoadInstruction
 * @brief Represents the LOAD instruction in the RAM machine, which loads the value of the operand into register R0 (the accumulator).
 */
class LoadInstruction : public Instruction {
 public:
  explicit LoadInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}
  
  void execute(RAMMachine& machine) override {
    auto& mem = machine.getMemory();
    mem.write(0, operand_->getValue(machine));
  }
 private:
  std::unique_ptr<Operand> operand_;
};

#endif // LOAD_INSTRUCTION_H