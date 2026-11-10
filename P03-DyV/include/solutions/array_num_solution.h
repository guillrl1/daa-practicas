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

#ifndef ARRAY_NUM_SOLUTION_H
#define ARRAY_NUM_SOLUTION_H

#include "solution.h"
#include <vector>

/**
 * @class ArrayNumSolution
 * @brief Concrete class representing a specific solution to the sorting problem,
 *        where the solution consists of a vector of integers that has been sorted. It
 *        inherits from the abstract base class Solution and provides a constructor
 *        to initialize the sorted vector of integers.
 */
class ArrayNumSolution : public Solution {
 public:
  ArrayNumSolution(const std::vector<int>& sorted_numbers) : sorted_numbers_(sorted_numbers) {}
  const std::vector<int>& GetSortedNumbers() const { return sorted_numbers_; }
  bool isFeasible() const override {
    for (size_t i = 1; i < sorted_numbers_.size(); ++i) {
      if (sorted_numbers_[i] < sorted_numbers_[i - 1]) {
        return false;
      }
    }
    return true;
  }
 private:
  std::vector<int> sorted_numbers_;
};

#endif // ARRAY_NUM_SOLUTION_H