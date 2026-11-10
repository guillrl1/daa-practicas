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

#ifndef EMPLOYEES_DP_SOLVER_H
#define EMPLOYEES_DP_SOLVER_H

#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"

#include <vector>
#include <memory>
#include <iostream>

/**
 * @class EmployeesDPSolver
 * @brief Resuelve el problema de planificación para UN solo día usando
 *        Programación Dinámica. Diseñado para usarse como SmallSolver
 *        dentro de EmployeesDivideAndConquer.
 */
class EmployeesDPSolver : public Algorithm {
 public:
  /**
   * @brief Constructor. Recibe los parámetros globales necesarios para
   *        construir el EmployeesSolution final.
   * @param global_days_off Días de descanso globales de cada empleado.
   * @param global_horizon Horizonte total de planificación.
   */
  EmployeesDPSolver(const std::vector<unsigned int>& global_days_off, int global_horizon)
      : global_days_off_(global_days_off), global_horizon_(global_horizon) {}

  /**
   * @brief Resuelve una instancia de 1 día con Programación Dinámica.
   * @param instance Puntero a EmployeesInstance (debe tener exactamente 1 día).
   * @return EmployeesSolution con el horario óptimo para ese día.
   */
  std::unique_ptr<Solution> Solve(const Instance* instance) override;

 private:
  std::vector<unsigned int> global_days_off_;
  int global_horizon_;
};

#endif // EMPLOYEES_DP_SOLVER_H