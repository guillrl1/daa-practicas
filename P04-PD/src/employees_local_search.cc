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

#include <iostream>

#include "employees_local_search.h"

/**
 * @brief Implementación del método Solve de EmployeesLocalSearch.
 * @param instance Puntero a EmployeesInstance.
 * @return EmployeesSolution mejorada tras aplicar búsqueda local.
 */
inline std::unique_ptr<Solution> EmployeesLocalSearch::Solve(const Instance* instance) {
  const auto* emp_instance = dynamic_cast<const EmployeesInstance*>(instance);
  if (!emp_instance) return nullptr;

  auto base_result = base_solver_->Solve(instance);
  const auto* base_solution = dynamic_cast<const EmployeesSolution*>(base_result.get());
  if (!base_solution) return base_result;

  const auto& satisfaction = emp_instance->GetSatisfactionRates();
  const auto& minimum = emp_instance->GetRequiredEmployees();
  const auto& days_off = emp_instance->GetDaysOff();

  int total_employees = static_cast<int>(days_off.size());
  int total_days = static_cast<int>(minimum.size());

  std::vector<int> max_work_days(total_employees);
  for (int employee_index = 0; employee_index < total_employees; ++employee_index)
    max_work_days[employee_index] = total_days - static_cast<int>(days_off[employee_index]);

  FullSchedule schedule = base_solution->GetSchedule();

  std::vector<int> days_worked(total_employees, 0);
  for (const auto& day : schedule) {
    std::vector<bool> worked_today(total_employees, false);
    for (const auto& shift : day)
      for (const auto& [employee_id, sat] : shift)
        worked_today[employee_id] = true;
    for (int employee_index = 0; employee_index < total_employees; ++employee_index)
      if (worked_today[employee_index]) days_worked[employee_index]++;
  }

  double objective_before = base_solution->ObjectiveFunction();

  int repaired = RepairUncoveredShifts(schedule, minimum, satisfaction, max_work_days, days_worked);
  int total_swaps = ImproveBySwaps(schedule, satisfaction);

  auto improved_solution = std::make_unique<EmployeesSolution>(schedule, days_off, total_days);

  std::cout << "[LocalSearch] f(x) base: " << objective_before
            << " -> f(x) mejorada: " << improved_solution->ObjectiveFunction()
            << "  (+" << improved_solution->ObjectiveFunction() - objective_before << ")"
            << "  turnos reparados: " << repaired
            << "  intercambios: " << total_swaps << "\n";

  return improved_solution;
}

/**
 * @brief Fase 1 de la búsqueda local: cubre turnos descubiertos asignando empleados 
 * libres con días disponibles.
 * @param schedule Horario actual (modificado in-place).
 * @param minimum Requerimientos mínimos de empleados por día y turno.
 * @param satisfaction Matriz de satisfacción por empleado, día y turno.
 * @param max_work_days Días máximos que puede trabajar cada empleado.
 * @param days_worked Días ya trabajados por cada empleado (modificado in-place).
 * @return Número de turnos reparados.
 */
inline int EmployeesLocalSearch::RepairUncoveredShifts(
    FullSchedule& schedule,
    const std::vector<std::vector<unsigned int>>& minimum,
    const std::vector<std::vector<std::vector<int>>>& satisfaction,
    const std::vector<int>& max_work_days,
    std::vector<int>& days_worked) {

  int total_days = static_cast<int>(schedule.size());
  int total_employees = static_cast<int>(max_work_days.size());
  int repaired = 0;

  for (int day = 0; day < total_days; ++day) {
    int total_shifts = static_cast<int>(schedule[day].size());

    std::vector<bool> worked_today(total_employees, false);
    for (int shift = 0; shift < total_shifts; ++shift)
      for (const auto& [employee_id, sat] : schedule[day][shift])
        worked_today[employee_id] = true;

    for (int shift = 0; shift < total_shifts; ++shift) {
      int assigned = static_cast<int>(schedule[day][shift].size());
      int required = static_cast<int>(minimum[day][shift]);

      while (assigned < required) {
        int best_employee = -1, best_sat = -1;
        for (int employee_index = 0; employee_index < total_employees; ++employee_index) {
          if (!worked_today[employee_index] && days_worked[employee_index] < max_work_days[employee_index]) {
            int employee_sat = satisfaction[employee_index][day][shift];
            if (employee_sat > best_sat) {
              best_sat = employee_sat;
              best_employee = employee_index;
            }
          }
        }
        if (best_employee == -1) break;

        schedule[day][shift].push_back({best_employee, best_sat});
        worked_today[best_employee] = true;
        days_worked[best_employee]++;
        ++assigned;
        ++repaired;
      }
    }
  }
  return repaired;
}

/**
 * @brief Fase 2 de la búsqueda local: intercambia empleados entre turnos del mismo día
 * si mejora la satisfacción total.
 * @param schedule Horario actual (modificado in-place).
 * @param satisfaction Matriz de satisfacción por empleado, día y turno.
 * @return Número de intercambios realizados.
 */
inline int EmployeesLocalSearch::ImproveBySwaps(
    FullSchedule& schedule,
    const std::vector<std::vector<std::vector<int>>>& satisfaction) {

  int total_days = static_cast<int>(schedule.size());
  int swaps = 0;

  for (int day = 0; day < total_days; ++day) {
    int total_shifts = static_cast<int>(schedule[day].size());

    for (int shift_a = 0; shift_a < total_shifts; ++shift_a) {
      for (size_t i = 0; i < schedule[day][shift_a].size(); ++i) {
        for (int shift_b = shift_a + 1; shift_b < total_shifts; ++shift_b) {
          for (size_t j = 0; j < schedule[day][shift_b].size(); ++j) {
            int employee_a = schedule[day][shift_a][i].first;
            int employee_b = schedule[day][shift_b][j].first;

            int sat_a_in_shift_a = satisfaction[employee_a][day][shift_a];
            int sat_b_in_shift_b = satisfaction[employee_b][day][shift_b];
            int sat_a_in_shift_b = satisfaction[employee_a][day][shift_b];
            int sat_b_in_shift_a = satisfaction[employee_b][day][shift_a];

            int gain = (sat_a_in_shift_b + sat_b_in_shift_a) - (sat_a_in_shift_a + sat_b_in_shift_b);
            if (gain > 0) {
              schedule[day][shift_a][i] = {employee_a, sat_a_in_shift_b};
              schedule[day][shift_b][j] = {employee_b, sat_b_in_shift_a};
              ++swaps;
            }
          }
        }
      }
    }
  }
  return swaps;
}