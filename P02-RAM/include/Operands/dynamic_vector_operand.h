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

#ifndef DYNAMIC_VECTOR_OPERAND_H
#define DYNAMIC_VECTOR_OPERAND_H

#include "operand.h"
#include "RAM_Machine.h"

/**
 * @class DynamicVectorOperand
 * @brief Represents a dynamic vector operand in the RAM machine, which uses one register as a base index and another register to determine the internal index at runtime.
 */
class DynamicVectorOperand : public Operand {
 public:
  DynamicVectorOperand(int base_reg, int index_reg)
    : base_reg_{base_reg}, index_reg_{index_reg} {}

  int getValue(RAMMachine& machine) const override {
    int dynamic_index = machine.getMemory().read(index_reg_);
    return machine.getMemory().read(base_reg_, dynamic_index);
  }

  void setValue(RAMMachine& machine, int value) const override {
    int dynamic_index = machine.getMemory().read(index_reg_);
    machine.getMemory().write(base_reg_, value, dynamic_index);
  }
  
  bool resolvesToR0(RAMMachine& machine) const override {
    return base_reg_ == 0;
  }
private:
  int base_reg_;
  int index_reg_;
};

#endif // DYNAMIC_VECTOR_OPERAND_H