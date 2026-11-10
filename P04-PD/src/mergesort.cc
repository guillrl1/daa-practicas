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

#include "mergesort.h"

/**
 * @brief Determines if the given instance is small enough to be solved directly.
 * @param instance A pointer to the instance to check.
 * @return true if the ArrayNumInstance contains 0 or 1 elements; false otherwise.
 */
bool MergeSort::Small(const Instance* instance) {
  const ArrayNumInstance* array_instance = dynamic_cast<const ArrayNumInstance*>(instance);
  if (array_instance == nullptr) {
    return false;
  }
  return array_instance->GetNumbers().size() <= 1;
}

/**
 * @brief Solves a small instance of the sorting problem directly by returning 
 *        the input vector as the sorted solution.
 * @param instance A pointer to the instance to solve.
 * @return A unique pointer to the solution containing the sorted vector.
 */
std::unique_ptr<Solution> MergeSort::SolveSmall(const Instance* instance) {
  const ArrayNumInstance* array_instance = dynamic_cast<const ArrayNumInstance*>(instance);
  if (array_instance == nullptr) {
    return nullptr;
  }
  auto small_solution = std::make_unique<ArrayNumSolution>(array_instance->GetNumbers());
  return small_solution;
}

/**
 * @brief Divides the given instance into two subinstances by splitting the input vector
 *        into two halves.
 * @param instance A pointer to the instance to divide.
 * @return A vector of unique pointers to the subinstances created from the original instance.
 */
std::vector<std::unique_ptr<Instance>> MergeSort::Divide(const Instance* instance) {
  const ArrayNumInstance* array_instance = dynamic_cast<const ArrayNumInstance*>(instance);
  if (array_instance == nullptr) {
    return {};
  }
  const std::vector<int>& numbers = array_instance->GetNumbers();
  size_t mid = numbers.size() / 2;
  std::vector<int> left(numbers.begin(), numbers.begin() + mid);
  std::vector<int> right(numbers.begin() + mid, numbers.end());
  std::vector<std::unique_ptr<Instance>> subinstances;
  subinstances.reserve(2);
  subinstances.push_back(std::make_unique<ArrayNumInstance>(left));
  subinstances.push_back(std::make_unique<ArrayNumInstance>(right));
  return subinstances;
}

/**
 * @brief Combines the sorted solutions from the subinstances into a single sorted solution 
 *        by merging the two sorted vectors.
 * @param partial_solution A vector of unique pointers to the partial solutions to combine.
 * @return A unique pointer to the combined solution containing the fully sorted vector.
 */
std::unique_ptr<Solution> MergeSort::Combine(std::vector<std::unique_ptr<Solution>> partial_solution) {
  if (partial_solution.size() != 2) {
    return nullptr;
  }
  auto* left_solution = dynamic_cast<ArrayNumSolution*>(partial_solution[0].get());
  auto* right_solution = dynamic_cast<ArrayNumSolution*>(partial_solution[1].get());
  if (left_solution == nullptr || right_solution == nullptr) {
    return nullptr;
  }
  const std::vector<int>& left_numbers = left_solution->GetSortedNumbers();
  const std::vector<int>& right_numbers = right_solution->GetSortedNumbers();
  std::vector<int> merged{};
  size_t i = 0;
  size_t j = 0;
  while (i < left_numbers.size() && j < right_numbers.size()) {
    merged.push_back(left_numbers[i] < right_numbers[j] ? left_numbers[i++] : right_numbers[j++]);
  }
  while (i < left_numbers.size()) {
    merged.push_back(left_numbers[i++]);
  }
  while (j < right_numbers.size()) {
    merged.push_back(right_numbers[j++]);
  }
  return std::make_unique<ArrayNumSolution>(merged);
}