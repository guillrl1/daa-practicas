/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 25/02/2026
 */

#ifndef GENERAL_ALGORITHM_H
#define GENERAL_ALGORITHM_H

#include "instance.h"
#include "solution.h"

#include <memory>

/**
 * @class Algorithm
 * @brief Abstract base class representing a general algorithm that can solve 
 *        a given instance of a problem.
 */
class Algorithm {
 public:
  virtual ~Algorithm() = default;
  virtual std::unique_ptr<Solution> Solve(const Instance* instance) = 0;
};

#endif // GENERAL_ALGORITHM_H