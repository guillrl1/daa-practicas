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

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <chrono>
#include <stdexcept>

#include "quicksort.h"
#include "mergesort.h"
#include "employees_instance.h"
#include "employees_algorithm.h"
#include "employees_solution.h"
#include "employees_solution_print.h"

/**
 * @brief Prints the contents of a vector to the console, with an optional limit 
 *        on the number of elements to display.
 * @param array The vector to print.
 * @param max_elements_to_print The maximum number of elements to print from the vector.
 */
void PrintVector(const std::vector<int>& array, size_t max_elements_to_print = 14) {
  std::cout << "Size " << array.size() << " [";
  for (size_t i = 0; i < array.size(); ++i) {
    if (i > max_elements_to_print) {
      std::cout << "... ";
      break;
    }
    std::cout << array[i];
    i != array.size() - 1 ? std::cout << ", " : std::cout << "";
  }
  std::cout << "]" << std::endl;
}

/**
 * @brief Generates a vector of random integers within a specified range.
 * @param size The number of random integers to generate.
 * @param min_value The minimum value for the random integers (inclusive).
 * @param max_value The maximum value for the random integers (inclusive).
 * @return A vector containing the generated random integers.
 */
std::vector<int> GenerateRandomVector(size_t size, int min_value = 0, int max_value = 100) {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> distribution(min_value, max_value);

  std::vector<int> random_int_vector(size);
  for (size_t i = 0; i < size; ++i) {
    random_int_vector[i] = distribution(generator);
  }
  return random_int_vector;
}

/**
 * @brief Executes the normal execution mode, which allows the user to choose a
 *        sorting algorithm, generates multiple test vectors of increasing size,
 *        and measures the execution time for each.
 * @return true if the execution was successful, false otherwise.
 */
bool NormalExecution() {
  const bool normal_execution = true;
  std::cout << "Choosing sorting algorithm: 1 for QuickSort, 2 for MergeSort: ";
  int choice{};
  std::cin >> choice;
  std::unique_ptr<DivideAndConquer> sorting_algorithm;
  if (choice == 1) {
    std::cout << "You chose QuickSort." << std::endl;
    sorting_algorithm = std::make_unique<QuickSort>();
  } else if (choice == 2) {
    std::cout << "You chose MergeSort." << std::endl;
    sorting_algorithm = std::make_unique<MergeSort>();
  } else {
    std::cerr << "Invalid choice. Exiting..." << std::endl;
    return !normal_execution;
  }

  const size_t max_element_num = 99999;
  std::vector<std::vector<int>> test_vectors = {
    GenerateRandomVector(10000, 0, max_element_num),
    GenerateRandomVector(20000, 0, max_element_num),
    GenerateRandomVector(50000, 0, max_element_num),
    GenerateRandomVector(100000, 0, max_element_num),
    GenerateRandomVector(200000, 0, max_element_num)
  };

  for (const auto& numbers : test_vectors) {
    std::cout << "Instance (unordered vector): ";
    PrintVector(numbers);
  }

  std::vector<double> execution_times_in_ms{};
  for (const auto& numbers : test_vectors) {
    const auto instance = std::make_unique<ArrayNumInstance>(numbers);
    auto start_time = std::chrono::high_resolution_clock::now();
    const auto result = sorting_algorithm->Solve(instance.get());
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    execution_times_in_ms.push_back(elapsed_seconds.count() * 1000);

    const auto* array_result = dynamic_cast<const ArrayNumSolution*>(result.get());
    if (array_result != nullptr) {
      std::cout << "Solution (ordered vector): ";
      PrintVector(array_result->GetSortedNumbers());
    } else {
      std::cerr << "Critical error: The returned solution is not of type ArrayNumSolution." 
                << std::endl;
      return !normal_execution;
    }
  }
  std::cout << "Execution times (ms): ";
  for (const auto& time : execution_times_in_ms) {
    std::cout << time << " ";
  }
  std::cout << std::endl;
  return normal_execution;
}

/**
 * @brief Executes the debug execution mode, which allows the user to choose a
 *        sorting algorithm, input the size of a test vector, and displays the
 *        unordered and ordered vectors.
 * @return true if the execution was successful, false otherwise.
 */
bool DebugExecution() {
  bool debug_execution = true;
  std::cout << "Choosing sorting algorithm: 1 for QuickSort, 2 for MergeSort: ";
  int choice{};
  std::cin >> choice;
  std::unique_ptr<DivideAndConquer> sorting_algorithm;
  if (choice == 1) {
    std::cout << "You chose QuickSort." << std::endl;
    sorting_algorithm = std::make_unique<QuickSort>();
  } else if (choice == 2) {
    std::cout << "You chose MergeSort." << std::endl;
    sorting_algorithm = std::make_unique<MergeSort>();
  } else {
    std::cerr << "Invalid choice. Exiting..." << std::endl;
    return !debug_execution;
  }

  const size_t max_element_num = 999;
  std::cout << "Enter the size of the array to sort: ";
  size_t array_size{};
  std::cin >> array_size;
  const auto numbers = GenerateRandomVector(array_size, 0, max_element_num);
  std::cout << "Instance (unordered vector): ";
  PrintVector(numbers);

  const auto instance = std::make_unique<ArrayNumInstance>(numbers);
  const auto result = sorting_algorithm->Solve(instance.get());
  const auto* array_result = dynamic_cast<const ArrayNumSolution*>(result.get());
  if (array_result != nullptr) {
    std::cout << "Solution (ordered vector): ";
    PrintVector(array_result->GetSortedNumbers());
  } else {
    std::cerr << "Critical error: The returned solution is not of type ArrayNumSolution." 
              << std::endl;
    return !debug_execution;
  }
  return debug_execution;
}

/**
 * @brief Ejecuta la prueba del problema de los empleados leyendo un JSON por consola.
 * @return true si la ejecución fue exitosa, false en caso de error.
 */
bool TestEmployeesAlgorithm() {
  std::string json_filename;
  std::cout << "Introduce la ruta del archivo JSON de la instancia: ";
  std::cin >> json_filename;

  try {
    std::cout << "\nCargando instancia desde " << json_filename << "..." << std::endl;
    EmployeesInstance instance(json_filename);
    int horizon = instance.GetRequiredEmployees().size();
    auto algorithm = std::make_unique<EmployeesAlgorithm>(instance.GetDaysOff(), horizon, instance.GetSatisfactionRates());

    std::cout << "Ejecutando algoritmo Divide y Vencerás..." << std::endl;
    auto result = algorithm->Solve(&instance);

    const auto* solution = dynamic_cast<const EmployeesSolution*>(result.get());
    if (solution != nullptr) {
      std::cout << "Resultados obtenidos:\n" << std::endl;
      EmployeesSolutionPrint printer(*solution);
      printer.PrintAsJSON();
    }
  } catch (const std::exception& exception) {
    std::cerr << "Error durante la ejecución: " << exception.what() << std::endl;
    return false;
  }
  return true;
}

int main() {
  std::cout << "Elige el modo de ejecución:\n"
            << "1 - Ejecución Normal (Parte 1)\n"
            << "2 - Ejecución Debug  (Parte 1)\n"
            << "3 - Problema de Empleados (Parte 2)\n"
            << "Opción: ";
  int mode_choice{};
  std::cin >> mode_choice;
  if (mode_choice == 1) {
    return NormalExecution() ? EXIT_SUCCESS : EXIT_FAILURE;
  } 
  if (mode_choice == 2) {
    return DebugExecution() ? EXIT_SUCCESS : EXIT_FAILURE;
  } 
  if (mode_choice == 3) {
    return TestEmployeesAlgorithm() ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  std::cerr << "Opción no válida. Saliendo..." << std::endl;
  return EXIT_FAILURE;
}