/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 2: Simulador de la Máquina RAM
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 24/02/2026
 */

#ifndef MODULE_INSTRUCTION_H
#define MOUDLE_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

#include <memory>

class ModuleInstruction : public Instruction {
 public:
  explicit ModuleInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}

  void execute(RAMMachine& machine) override {
    auto& mem = machine.getMemory();
    int result = mem.read(0) % operand_->getValue(machine);
    mem.write(0, result);
  }
 private:
  std::unique_ptr<Operand> operand_;
};

#endif // MODULE_INSTRUCTION_H