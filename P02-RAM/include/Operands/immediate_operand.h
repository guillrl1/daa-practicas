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

#ifndef IMMEDIATE_OPERAND_H
#define IMMEDIATE_OPERAND_H

#include "operand.h"
#include <stdexcept>

/**
 * @class ImmediateOperand
 * @brief Represents an immediate operand in the RAM machine, which holds a constant integer value that cannot be modified at runtime.
 */
class ImmediateOperand : public Operand {
 public:
  explicit ImmediateOperand(int val) : val_{val} {}

  int getValue(RAMMachine& machine) const override {
    return val_;
  }

  void setValue(RAMMachine& machine, int value) const override {
    throw std::invalid_argument("Error crítico: Intento de escritura en un operando inmediato (constante) en la línea " + std::to_string(machine.getPc()));
  }
  
  bool resolvesToR0(RAMMachine& machine) const override {
    return false;
  }
 private:
  int val_;
};

#endif // IMMEDIATE_OPERAND_H