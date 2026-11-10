/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 25/02/2026
 */

#ifndef SOLUTION_H
#define SOLUTION_H

/**
 * @class Solution
 * @brief Abstract base class representing a general solution to a problem solved 
 *        by an algorithm.
 */
class Solution {
 public:
  virtual ~Solution() = default;
  virtual bool isFeasible() const = 0;
};

#endif // SOLUTION_H