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

#include "employees_dp_multiday.h"

/**
* @brief Implementación del método Solve de EmployeesDPMultiday. Resuelve el problema de asignación
* de empleados a turnos para MÚLTIPLES días.
* @param instance Puntero a EmployeesInstance.
* @return EmployeesSolution con el horario planificado para todos los días.
*/
inline std::vector<std::vector<std::pair<int,int>>>
EmployeesDPMultiday::SolveOneDay(
    const std::vector<std::vector<int>>& sat_day,
    const std::vector<unsigned int>& min_day,
    const std::vector<int>& available,
    int total_shifts) {

  std::vector<std::vector<std::pair<int,int>>> day_schedule(total_shifts);

  int total_available = static_cast<int>(available.size());
  if (total_available == 0) return day_schedule;

  std::vector<int> slots;
  for (int shift = 0; shift < total_shifts; ++shift)
    for (unsigned int seat = 0; seat < min_day[shift]; ++seat)
      slots.push_back(shift);

  int total_slots = static_cast<int>(slots.size());
  if (total_slots == 0) return day_schedule;

  // dp_table[num_employees_considered][num_slots_considered]
  std::vector<std::vector<int>> dp_table(
      total_available + 1, std::vector<int>(total_slots + 1, 0));

  for (int num_employees_considered = 1; num_employees_considered <= total_available; ++num_employees_considered) {
    int employee_global_index = available[num_employees_considered - 1];
    for (int num_slots_considered = 1; num_slots_considered <= total_slots; ++num_slots_considered) {
      int current_shift = slots[num_slots_considered - 1];
      int employee_sat = sat_day[employee_global_index][current_shift];

      dp_table[num_employees_considered][num_slots_considered] = std::max(
          dp_table[num_employees_considered - 1][num_slots_considered],
          dp_table[num_employees_considered - 1][num_slots_considered - 1] + employee_sat);
    }
  }

  // Backtracking para reconstruir el horario
  std::vector<bool> employee_already_assigned(total_available, false);
  int remaining_emp = total_available;
  int remaining_slots = total_slots;

  while (remaining_emp > 0 && remaining_slots > 0) {
    bool employee_was_assigned =
        dp_table[remaining_emp][remaining_slots] != dp_table[remaining_emp - 1][remaining_slots];

    if (employee_was_assigned) {
      if (!employee_already_assigned[remaining_emp - 1]) {
        int current_shift = slots[remaining_slots - 1];
        int employee_global_index = available[remaining_emp - 1];
        day_schedule[current_shift].push_back({employee_global_index, sat_day[employee_global_index][current_shift]});
        employee_already_assigned[remaining_emp - 1] = true;
      }
      --remaining_slots;
    }
    --remaining_emp;
  }
  return day_schedule;
}

/**
* @brief Implementación del método Solve de EmployeesDPMultiday. Resuelve el problema de asignación
* de empleados a turnos para MÚLTIPLES días. Para cada día, llama a SolveOneDay con los empleados disponibles y luego actualiza los días trabajados.
* @param instance Puntero a EmployeesInstance.
* @return EmployeesSolution con el horario planificado para todos los días.
*/
inline std::unique_ptr<Solution> EmployeesDPMultiday::Solve(const Instance* instance) {
  const auto* emp_instance = dynamic_cast<const EmployeesInstance*>(instance);
  if (!emp_instance) return nullptr;

  const auto& satisfaction = emp_instance->GetSatisfactionRates();
  const auto& minimum = emp_instance->GetRequiredEmployees();
  const auto& days_off = emp_instance->GetDaysOff();

  int total_employees = static_cast<int>(days_off.size());
  int total_days = static_cast<int>(minimum.size());
  int total_shifts = static_cast<int>(minimum[0].size());

  std::vector<int> max_work_days(total_employees);
  for (int employee_index = 0; employee_index < total_employees; ++employee_index)
    max_work_days[employee_index] = total_days - static_cast<int>(days_off[employee_index]);

  std::vector<int> days_worked(total_employees, 0);

  using DaySchedule = std::vector<std::vector<std::pair<int,int>>>;
  using FullSchedule = std::vector<DaySchedule>;
  FullSchedule schedule(total_days, DaySchedule(total_shifts));

  for (int day = 0; day < total_days; ++day) {
    // Solo empleados que aún tienen días disponibles
    std::vector<int> available;
    for (int employee_index = 0; employee_index < total_employees; ++employee_index)
      if (days_worked[employee_index] < max_work_days[employee_index])
        available.push_back(employee_index);

    std::vector<std::vector<int>> sat_day(total_employees, std::vector<int>(total_shifts, 0));
    for (int employee_index = 0; employee_index < total_employees; ++employee_index)
      for (int shift = 0; shift < total_shifts; ++shift)
        sat_day[employee_index][shift] = satisfaction[employee_index][day][shift];

    schedule[day] = SolveOneDay(sat_day, minimum[day], available, total_shifts);

    std::vector<bool> worked_today(total_employees, false);
    for (int shift = 0; shift < total_shifts; ++shift)
      for (const auto& [employee_id, sat] : schedule[day][shift])
        worked_today[employee_id] = true;
    for (int employee_index = 0; employee_index < total_employees; ++employee_index)
      if (worked_today[employee_index]) days_worked[employee_index]++;
  }

  return std::make_unique<EmployeesSolution>(schedule, days_off, total_days);
}