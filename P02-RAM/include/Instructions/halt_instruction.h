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

#ifndef HALT_INSTRUCTION_H
#define HALT_INSTRUCTION_H

#include "instruction.h"

/**
 * @class HaltInstruction
 * @brief Represents the HALT instruction in the RAM machine, which stops the execution of the
 *        program when executed.
 */
class HaltInstruction : public Instruction {
 public:
  HaltInstruction() = default;

  void execute(RAMMachine& machine) override {
    machine.halt();
  }
};

#endif // HALT_INSTRUCTION_H