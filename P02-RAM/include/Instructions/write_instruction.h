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

#ifndef WRITE_INSTRUCTION_H
#define WRITE_INSTRUCTION_H

#include "instruction.h"
#include "operand.h"

#include <stdexcept>

/**
 * @class WriteInstruction
 * @brief Represents the WRITE instruction in the RAM machine, which writes the value of the operand to the output tape.
 */
class WriteInstruction : public Instruction {
public:
  explicit WriteInstruction(std::unique_ptr<Operand> op) : operand_{std::move(op)} {}
  
  void execute(RAMMachine& machine) override {
    if (operand_->resolvesToR0(machine)) {
        throw std::invalid_argument("WRITE: No se puede leer desde el registro R0 (acumulador). Error en la línea " + std::to_string(machine.getPc()));
    }
    machine.getOutputTape().write(operand_->getValue(machine));
  }
private:
  std::unique_ptr<Operand> operand_;
};

#endif // WRITE_INSTRUCTION_H