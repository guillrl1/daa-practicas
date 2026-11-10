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

#ifndef DIVIDE_Y_VENCERAS_H
#define DIVIDE_Y_VENCERAS_H

#include "algorithm.h"

#include <vector>

/**
 * @class DivideAndConquer
 * @brief Abstract base class representing a general divide and conquer algorithm
 *        that can solve a given instance of a problem by recursively dividing it
 *        into smaller subinstances, solving those subinstances, and then combining
 *        the results to form a solution to the original instance.
 */
class DivideAndConquer : Algorithm {
 public:
  std::unique_ptr<Solution> Solve(const Instance* instance) override {
    if (Small(instance)) {
      return SolveSmall(instance);
    }
    auto subinstances = Divide(instance);
    std::vector<std::unique_ptr<Solution>> partialSolutions{};
    for (auto& sub : subinstances) {
        partialSolutions.push_back(Solve(sub.get()));
    }
    return Combine(std::move(partialSolutions));
  }
 private:
  virtual bool Small(const Instance* instance) = 0;
  virtual std::unique_ptr<Solution> SolveSmall(const Instance* instance) = 0;
  virtual std::vector<std::unique_ptr<Instance>> Divide(const Instance* instance) = 0;
  virtual std::unique_ptr<Solution> Combine(std::vector<std::unique_ptr<Solution>> partialSolutions) = 0;
};

#endif // DIVIDE_Y_VENCERAS_H