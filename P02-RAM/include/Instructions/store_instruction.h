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

#ifndef STORE_INSTRUCTION_H
#define STORE_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

/**
 * @class StoreInstruction
 * @brief Represents the STORE instruction in the RAM machine, which stores the value in register R0 (the accumulator) into the location specified by the operand.
 */
class StoreInstruction : public Instruction {
 public:
  explicit StoreInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}
  
  void execute(RAMMachine& machine) override {
    int value = machine.getMemory().read(0);
    operand_->setValue(machine, value);
  }
 private:
  std::unique_ptr<Operand> operand_;
};

#endif // STORE_INSTRUCTION_H