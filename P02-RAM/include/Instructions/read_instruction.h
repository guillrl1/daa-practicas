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

#ifndef READ_INSTRUCTION_H
#define READ_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

#include <stdexcept>

/**
 * @class ReadInstruction
 * @brief Represents the READ instruction in the RAM machine, which reads a value from the input tape and stores it in the location specified by the operand.
 */
class ReadInstruction : public Instruction {
 public:
  explicit ReadInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}
  
  void execute(RAMMachine& machine) override {
    if (operand_->resolvesToR0(machine)) {
      throw std::invalid_argument("READ: No se puede escribir en el registro R0 (acumulador). Error en la línea " + std::to_string(machine.getPc()));
    }
    const int value = machine.getInputTape().read();
    operand_->setValue(machine, value);
  }
 private:
  std::unique_ptr<Operand> operand_;
};

#endif // READ_INSTRUCTION_H