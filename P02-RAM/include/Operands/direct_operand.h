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

#ifndef DIRECT_OPERAND_H
#define DIRECT_OPERAND_H

#include "operand.h"
#include "RAM_Machine.h"

/**
 * @class DirectOperand
 * @brief Represents a direct operand in the RAM machine, which directly references a register index.
 */
class DirectOperand : public Operand {
 public:
  explicit DirectOperand(int reg) : reg_{reg} {}

  int getValue(RAMMachine& machine) const override {
    return machine.getMemory().read(reg_);
  }

  void setValue(RAMMachine& machine, int value) const override {
    machine.getMemory().write(reg_, value);
  }
  
  bool resolvesToR0(RAMMachine& machine) const override {
    return reg_ == 0;
  }
 private:
  int reg_;
};

#endif // DIRECT_OPERAND_H