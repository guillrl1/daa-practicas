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

#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "divide_and_conquer.h"
#include "array_num_instance.h"
#include "array_num_solution.h"

/**
 * @class QuickSort
 * @brief Concrete class implementing the Quick Sort algorithm using the divide 
 *        and conquer approach. It inherits from the DivideAndConquer class and 
 *        provides specific implementations for the Small, SolveSmall, Divide
 *        and Combine methods.
 */
class QuickSort : public DivideAndConquer {
 private:
  bool Small(const Instance* instance) override;
  std::unique_ptr<Solution> SolveSmall(const Instance* instance) override;
  std::vector<std::unique_ptr<Instance>> Divide(const Instance* instance) override;
  std::unique_ptr<Solution> Combine(std::vector<std::unique_ptr<Solution>> partial_solutions) override;
};

#endif // QUICKSORT_H