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

#ifndef INDIRECT_OPERAND_H
#define INDIRECT_OPERAND_H

#include "operand.h"
#include "RAM_Machine.h"

/**
 * @class IndirectOperand
 * @brief Represents an indirect operand in the RAM machine, which uses the value of a register as an address to read from or write to memory.
 */
class IndirectOperand : public Operand {
 public:
  explicit IndirectOperand(int reg) : reg_{reg} {}

  int getValue(RAMMachine& machine) const override {
    int address = machine.getMemory().read(reg_);
    return machine.getMemory().read(address);
  }

  void setValue(RAMMachine& machine, int value) const override {
    int address = machine.getMemory().read(reg_);
    machine.getMemory().write(address, value);
  }
  
  bool resolvesToR0(RAMMachine& machine) const override {
    return machine.getMemory().read(reg_) == 0;
  }
 private:
    int reg_;
};

#endif // INDIRECT_OPERAND_H