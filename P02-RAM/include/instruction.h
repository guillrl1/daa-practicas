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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H 

#include <vector>

class RAMMachine;

/**
 * @class Instruction
 * @brief Abstract base class representing an instruction in the RAM machine.
 */
class Instruction {
 public: 
  virtual ~Instruction() = default;
  virtual void execute(RAMMachine& machine) = 0;
};

#endif // INSTRUCTION_H