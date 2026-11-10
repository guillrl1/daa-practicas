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

#include "quicksort.h"

/**
 * @brief Determines if the given instance is small enough to be solved directly.
 * @param instance A pointer to the instance to check.
 * @return true if the ArrayNumInstance contains 0 or 1 elements, or if all 
 *         elements are the same; false otherwise.
 */
bool QuickSort::Small(const Instance* instance) {
  const ArrayNumInstance* array_instance = dynamic_cast<const ArrayNumInstance*>(instance);
  if (array_instance == nullptr) {
    return false;
  }
  const std::vector<int>& nums = array_instance->GetNumbers();
  if (nums.size() <= 1) {
    return true;
  }
  return std::equal(nums.begin() + 1, nums.end(), nums.begin());
}

/**
 * @brief Solves a small instance of the sorting problem directly by returning 
 *        the input vector as the sorted solution.
 * @param instance A pointer to the instance to solve.
 * @return A unique pointer to the solution containing the sorted vector.
 */
std::unique_ptr<Solution> QuickSort::SolveSmall(const Instance* instance) {
  const ArrayNumInstance* array_instance = dynamic_cast<const ArrayNumInstance*>(instance);
  if (array_instance == nullptr) {
    return nullptr;
  }
  auto small_solution = std::make_unique<ArrayNumSolution>(array_instance->GetNumbers());
  return small_solution;
}

/**
 * @brief Divides the given instance into three subinstances based on a pivot value,
 *        creating subinstances for values less than, equal to, and greater than the pivot.
 * @param instance A pointer to the instance to divide.
 * @return A vector of unique pointers to the subinstances created from the original instance.
 */
std::vector<std::unique_ptr<Instance>> QuickSort::Divide(const Instance* instance) {
  const ArrayNumInstance* array_instance = dynamic_cast<const ArrayNumInstance*>(instance);
  if (array_instance == nullptr) {
    return {};
  }
  const std::vector<int>& numbers = array_instance->GetNumbers();
  // Pivot choice: middle element (arbitrary choice)
  const int pivot = numbers[numbers.size() / 2];
  std::vector<int> less{};
  std::vector<int> equal{};
  std::vector<int> greater{};
  for (const int num : numbers) {
    if (num < pivot) {
      less.push_back(num);
    } else if (num == pivot) {
      equal.push_back(num);
    } else {
      greater.push_back(num);
    }
  }
  std::vector<std::unique_ptr<Instance>> subinstances;
  subinstances.push_back(std::make_unique<ArrayNumInstance>(less));
  subinstances.push_back(std::make_unique<ArrayNumInstance>(equal));
  subinstances.push_back(std::make_unique<ArrayNumInstance>(greater));
  return subinstances;
}

/**
 * @brief Combines the sorted solutions from the subinstances into a single sorted solution 
 *        by concatenating the sorted vectors from the "less", "equal", and "greater" subinstances.
 * @param partial_solutions A vector of unique pointers to the partial solutions to combine.
 * @return A unique pointer to the combined solution containing the fully sorted vector.
 */
std::unique_ptr<Solution> QuickSort::Combine(std::vector<std::unique_ptr<Solution>> partial_solutions) {
  std::vector<int> sorted_numbers{};
  for (const auto& partial_solution : partial_solutions) {
    const ArrayNumSolution* array_solution = dynamic_cast<const ArrayNumSolution*>(partial_solution.get());
    if (array_solution) {
      const std::vector<int>& part_sorted = array_solution->GetSortedNumbers();
      sorted_numbers.insert(sorted_numbers.end(), part_sorted.begin(), part_sorted.end());
    }
  }
  return std::make_unique<ArrayNumSolution>(sorted_numbers);
}
