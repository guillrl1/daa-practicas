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

#ifndef DIV_INSTRUCTION_H
#define DIV_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

#include <memory>

/**
 * @class DivInstruction
 * @brief Represents the DIV instruction in the RAM machine, which divides the value in register R0 (the accumulator) by the value of the operand and stores the result back in R0.
 */
class DivInstruction : public Instruction {
 public:
  explicit DivInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}
  
  void execute(RAMMachine& machine) override {
    auto& mem = machine.getMemory();
    int divisor = operand_->getValue(machine);
    
    if (divisor == 0) {
      throw std::runtime_error("Error: División por cero en DIV instruction.");
    }

    int result = mem.read(0) / divisor;
    mem.write(0, result);
  }
 private:
  std::unique_ptr<Operand> operand_;
};

#endif // DIV_INSTRUCTION_H