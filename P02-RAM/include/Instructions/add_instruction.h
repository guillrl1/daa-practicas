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

#ifndef ADD_INSTRUCTION_H
#define ADD_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

#include <memory>

/**
 * @class AddInstruction
 * @brief Represents the ADD instruction in the RAM machine, which adds the value of the operand to the value in register R0 (the accumulator) and stores the result back in R0.
 */
class AddInstruction : public Instruction {
 public:
  explicit AddInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}
  
  void execute(RAMMachine& machine) override {
    auto& mem = machine.getMemory();
    int result = mem.read(0) + operand_->getValue(machine);
    mem.write(0, result);
  }
private:
  std::unique_ptr<Operand> operand_;
};

#endif // ADD_INSTRUCTION_H