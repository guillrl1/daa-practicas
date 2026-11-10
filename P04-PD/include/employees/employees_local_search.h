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

#ifndef EMPLOYEES_LOCAL_SEARCH_H
#define EMPLOYEES_LOCAL_SEARCH_H

#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

class EmployeesLocalSearch : public Algorithm {
public:
  explicit EmployeesLocalSearch(std::unique_ptr<Algorithm> base_solver)
      : base_solver_(std::move(base_solver)) {}

  std::unique_ptr<Solution> Solve(const Instance* instance) override;

private:
  std::unique_ptr<Algorithm> base_solver_;

  using ShiftAssignment = std::vector<std::pair<int,int>>;
  using DaySchedule = std::vector<ShiftAssignment>;
  using FullSchedule = std::vector<DaySchedule>;

  // Fase 1: cubre turnos descubiertos asignando empleados libres con días disponibles.
  int RepairUncoveredShifts(
      FullSchedule& schedule,
      const std::vector<std::vector<unsigned int>>& minimum,
      const std::vector<std::vector<std::vector<int>>>& satisfaction,
      const std::vector<int>& max_work_days,
      std::vector<int>& days_worked);

  // Fase 2: intercambia empleados entre turnos del mismo día si mejora la satisfacción total.
  int ImproveBySwaps(
      FullSchedule& schedule,
      const std::vector<std::vector<std::vector<int>>>& satisfaction);
};

#endif // EMPLOYEES_LOCAL_SEARCH_H