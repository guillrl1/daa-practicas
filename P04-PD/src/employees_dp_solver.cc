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

#include "employees_dp_solver.h"

/**
 * @brief Implementación del método Solve de EmployeesDPSolver.
 *        Resuelve el problema de asignación de empleados a turnos para UN solo día
 *        usando una tabla de Programación Dinámica. Luego reconstruye la solución.
 */
inline std::unique_ptr<Solution> EmployeesDPSolver::Solve(const Instance* instance) {
  const auto* emp_instance = dynamic_cast<const EmployeesInstance*>(instance);
  if (!emp_instance) return nullptr;

  const auto& satisfaction = emp_instance->GetSatisfactionRates();
  const auto& minimum = emp_instance->GetRequiredEmployees();

  int total_employees = static_cast<int>(satisfaction.size());
  int total_shifts = static_cast<int>(minimum[0].size());

  struct Slot { int shift_index; };
  std::vector<Slot> slots;
  for (int shift = 0; shift < total_shifts; ++shift) {
    for (unsigned int seat = 0; seat < minimum[0][shift]; ++seat) {
      slots.push_back({shift});
    }
  }
  int total_slots = static_cast<int>(slots.size());

  if (total_slots == 0) {
    std::vector<std::vector<std::vector<std::pair<int,int>>>> empty_schedule(
        1, std::vector<std::vector<std::pair<int,int>>>(total_shifts));
    return std::make_unique<EmployeesSolution>(empty_schedule, global_days_off_, global_horizon_);
  }

  std::vector<std::vector<int>> dp_table(
      total_employees + 1, std::vector<int>(total_slots + 1, 0));

  for (int num_employees_considered = 1; num_employees_considered <= total_employees; ++num_employees_considered) {
    for (int num_slots_considered = 1; num_slots_considered <= total_slots; ++num_slots_considered) {
      int current_shift = slots[num_slots_considered - 1].shift_index;
      int employee_index = num_employees_considered - 1;
      int employee_sat = satisfaction[employee_index][0][current_shift];

      int sat_without_employee = dp_table[num_employees_considered - 1][num_slots_considered];
      int sat_with_employee = dp_table[num_employees_considered - 1][num_slots_considered - 1] + employee_sat;

      dp_table[num_employees_considered][num_slots_considered] = std::max(sat_without_employee, sat_with_employee);
    }
  }

  std::vector<std::vector<std::vector<std::pair<int,int>>>> schedule(
      1, std::vector<std::vector<std::pair<int,int>>>(total_shifts));

  std::vector<bool> employee_already_assigned(total_employees, false);
  int remaining_employees = total_employees;
  int remaining_slots = total_slots;

  while (remaining_employees > 0 && remaining_slots > 0) {
    int current_shift = slots[remaining_slots - 1].shift_index;
    int employee_index = remaining_employees - 1;
    int employee_sat = satisfaction[employee_index][0][current_shift];

    bool employee_was_assigned =
        dp_table[remaining_employees][remaining_slots] != dp_table[remaining_employees - 1][remaining_slots];

    if (employee_was_assigned) {
      if (!employee_already_assigned[employee_index]) {
        schedule[0][current_shift].push_back({employee_index, employee_sat});
        employee_already_assigned[employee_index] = true;
      }
      --remaining_slots;
    }
    --remaining_employees;
  }
  return std::make_unique<EmployeesSolution>(schedule, global_days_off_, global_horizon_);
}