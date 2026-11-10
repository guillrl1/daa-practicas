/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 4: Experimentación con Programación Dinámica y Patrón Factory
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 08/03/2026
 */

#ifndef EMPLOYEES_DP_MULTIDAY_H
#define EMPLOYEES_DP_MULTIDAY_H

#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"

#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

/**
 * @class EmployeesDPMultiday
 * @brief Algoritmo de Programación Dinámica para planificación de empleados en MÚLTIPLES días. 
 * Resuelve cada día con un DP de slots sobre los empleados disponibles, teniendo en cuenta los 
 * días máximos de trabajo de cada empleado. 
 */
class EmployeesDPMultiday : public Algorithm {
public:
  EmployeesDPMultiday() = default;
  std::unique_ptr<Solution> Solve(const Instance* instance) override;

private:
  std::vector<std::vector<std::pair<int,int>>> SolveOneDay(
      const std::vector<std::vector<int>>& sat_day,
      const std::vector<unsigned int>& min_day,
      const std::vector<int>& available,
      int total_shifts);
};

#endif // EMPLOYEES_DP_MULTIDAY_H