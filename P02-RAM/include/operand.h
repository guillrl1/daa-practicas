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

#ifndef OPERAND_H
#define OPERAND_H

#include <stdexcept>

class RAMMachine;

/**
 * @class Operand
 * @brief Abstract base class representing an operand in a RAM instruction.
 *        Defines the interface for getting and setting values, as well as
 *        checking if the operand resolves to R0 (the accumulator).
 */
class Operand {
 public:
  virtual ~Operand() = default;
  virtual int getValue(RAMMachine& machine) const = 0;
  virtual void setValue(RAMMachine& machine, int value) const = 0;
  virtual bool resolvesToR0(RAMMachine& machine) const = 0;
};

class ImmediateOperand;
class DirectOperand;
class IndirectOperand;
class VectorOperand;
class DynamicVectorOperand;

#endif // OPERAND_H