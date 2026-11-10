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

#ifndef EMPLOYEES_GREEDY_SOLVER_H
#define EMPLOYEES_GREEDY_SOLVER_H

#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"

#include <vector>
#include <algorithm>
#include <memory>

class EmployeesGreedySolver : public Algorithm {
 public:
  EmployeesGreedySolver(const std::vector<unsigned int>& global_days_off, int global_horizon)
      : global_days_off_(global_days_off), global_horizon_(global_horizon) {}

  std::unique_ptr<Solution> Solve(const Instance* instance) override {
    const auto* emp_instance = dynamic_cast<const EmployeesInstance*>(instance);
    if (!emp_instance) return nullptr;

    const auto& satisfaction = emp_instance->GetSatisfactionRates();
    const auto& minimum = emp_instance->GetRequiredEmployees();
    int total_employees = static_cast<int>(satisfaction.size());
    int total_shifts = static_cast<int>(minimum[0].size());

    std::vector<std::vector<std::vector<std::pair<int,int>>>> schedule(
        1, std::vector<std::vector<std::pair<int,int>>>(total_shifts));

    std::vector<bool> worked_today(total_employees, false);

    for (int shift = 0; shift < total_shifts; ++shift) {
      int required = static_cast<int>(minimum[0][shift]);
      std::vector<std::pair<int,int>> candidates;
      for (int employee_index = 0; employee_index < total_employees; ++employee_index) {
        if (!worked_today[employee_index])
          candidates.push_back({satisfaction[employee_index][0][shift], employee_index});
      }
      std::sort(candidates.begin(), candidates.end(), std::greater<std::pair<int,int>>());
      for (int i = 0; i < required && i < static_cast<int>(candidates.size()); ++i) {
        int employee_id = candidates[i].second;
        int employee_sat = candidates[i].first;
        schedule[0][shift].push_back({employee_id, employee_sat});
        worked_today[employee_id] = true;
      }
    }
    return std::make_unique<EmployeesSolution>(schedule, global_days_off_, global_horizon_);
  }

 private:
  std::vector<unsigned int> global_days_off_;
  int global_horizon_;
};

#endif // EMPLOYEES_GREEDY_SOLVER_H