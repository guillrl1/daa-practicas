/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 08/03/2026
 * Descripción: Implementación de un algoritmo de Branch & Bound para el
 *              problema de asignación de empleados a turnos.
 *              El algoritmo explora el espacio de soluciones con poda basada
 *              en una cota superior admisible y factibilidad de descansos.
 */

#ifndef EMPLOYEES_BRANCH_AND_BOUND_H
#define EMPLOYEES_BRANCH_AND_BOUND_H

#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"

#include <vector>
#include <memory>
#include <limits>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <iomanip>

/**
 * @class EmployeesBranchAndBound
 * @brief Algoritmo de Branch & Bound para el problema de asignación de empleados a turnos.
 */
class EmployeesBranchAndBound : public Algorithm {
 public:
  EmployeesBranchAndBound() = default;
  std::unique_ptr<Solution> Solve(const Instance* instance) override;

 private:
  using ShiftAssignment = std::vector<std::pair<int,int>>;  // {emp_id, sat}
  using DaySchedule     = std::vector<ShiftAssignment>;
  using FullSchedule    = std::vector<DaySchedule>;

  int num_employees_ = 0;
  int num_days_      = 0;
  int num_shifts_    = 0;
  std::vector<unsigned int>                  days_off_;
  std::vector<int>                           max_work_days_;
  std::vector<std::vector<std::vector<int>>> satisfaction_;  // [emp][d][t]
  std::vector<std::vector<unsigned int>>     minimum_;       // [d][t]

  FullSchedule best_schedule_;
  double       best_objective_ = -std::numeric_limits<double>::infinity();
  long long    nodes_visited_  = 0;
  long long    nodes_pruned_   = 0;

  double ObjectiveFunction(const FullSchedule& schedule) const {
    double sat = 0.0, covered = 0.0;
    for (int d = 0; d < num_days_; ++d) {
      for (int t = 0; t < num_shifts_; ++t) {
        for (const auto& [emp, satisfaction] : schedule[d][t]) sat += satisfaction;
        if (static_cast<int>(schedule[d][t].size()) >= static_cast<int>(minimum_[d][t]))
          covered += 1.0;
      }
    }
    return sat + covered * 100.0;
  }

  /**
   * @brief Calcula una cota superior admisible del valor máximo alcanzable
   *        desde el nodo (day, shift) con los días disponibles dados.
   *        Resuelve una relajación greedy ignorando el conflicto de un turno por día
   *        pero respetando el total de días disponibles de cada empleado.
   *        Resultado: cota >= valor real alcanzable (admisible) ✓
   *
   * @param day Primer día pendiente de planificar.
   * @param shift Primer turno pendiente dentro de ese día.
   * @param days_worked Días ya trabajados por cada empleado.
   * @return Cota superior del valor máximo alcanzable desde este nodo.
   */
  double UpperBound(int day, int shift, const std::vector<int>& days_worked) const {
    std::vector<int> days_left(num_employees_);
    for (int emp = 0; emp < num_employees_; ++emp)
      days_left[emp] = max_work_days_[emp] - days_worked[emp];
    double bound = 0.0;

    for (int actual_day = day; actual_day < num_days_; ++actual_day) {
      int time_start = (actual_day == day) ? shift : 0;
      for (int actual_time = time_start; actual_time < num_shifts_; ++actual_time) {
        int required = static_cast<int>(minimum_[actual_day][actual_time]);

        if (required == 0) {
          bound += 100.0;
          continue;
        }

        std::vector<std::pair<int,int>> candidates;
        for (int emp = 0; emp < num_employees_; ++emp) {
          if (days_left[emp] > 0)
            candidates.push_back({satisfaction_[emp][actual_day][actual_time], emp});
        }
        std::sort(candidates.begin(), candidates.end(),
                  [](const auto& a, const auto& b){ return a.first > b.first; });

        int assigned = 0;
        for (auto& [sat, emp] : candidates) {
          if (assigned == required) break;
          bound += sat;
          --days_left[emp];
          ++assigned;
        }
        if (assigned >= required) bound += 100.0;
      }
    }
    return bound;
  }

  /**
   * @brief Genera todas las combinaciones de exactamente k empleados
   *        tomados de `available`, ordenadas de mayor a menor satisfacción
   *        total para que el B&B encuentre buenas soluciones cuanto antes.
   */
  std::vector<ShiftAssignment> Combinations(
      const std::vector<int>& available, int employee_count, int day, int shift) const {

    std::vector<ShiftAssignment> result;
    int available_num = static_cast<int>(available.size());

    // Turno sin requisito: única opción es no asignar a nadie
    if (employee_count == 0) { result.push_back({}); return result; }

    // No hay suficientes empleados libres: mejor esfuerzo
    if (employee_count > available_num) {
      ShiftAssignment effort;
      for (int emp : available)
        effort.push_back({emp, satisfaction_[emp][day][shift]});
      result.push_back(effort);
      return result;
    }

    // Generar C(n,k) en orden lexicográfico con índices
    std::vector<int> employees_index(employee_count);
    std::iota(employees_index.begin(), employees_index.end(), 0);

    while (true) {
      ShiftAssignment asgn;
      for (int i : employees_index)
        asgn.push_back({available[i], satisfaction_[available[i]][day][shift]});
      result.push_back(std::move(asgn));

      int i = employee_count - 1;
      while (i >= 0 && employees_index[i] == available_num - employee_count + i) --i;
      if (i < 0) break;
      ++employees_index[i];
      for (int j = i+1; j < employee_count; ++j) employees_index[j] = employees_index[j-1] + 1;
    }

    // Ordenar de mayor a menor satisfacción total → mejores primero
    std::sort(result.begin(), result.end(),
      [](const ShiftAssignment& a, const ShiftAssignment& b) {
        int sa = 0, sb = 0;
        for (const auto& [emp, satisfaction] : a) sa += satisfaction;
        for (const auto& [emp, satisfaction] : b) sb += satisfaction;
        return sa > sb;
      });

    return result;
  }

  void BnB(int day, int shift, double current_obj,
           FullSchedule& schedule,
           std::vector<int>& days_worked,
           std::vector<bool>& worked_today) {

    // Caso base: solución completa
    if (day == num_days_) {
      ++nodes_visited_;
      if (current_obj > best_objective_) {
        best_objective_ = current_obj;
        best_schedule_  = schedule;
      }
      return;
    }

    // Poda 2: cota superior admisible
    double ub = current_obj + UpperBound(day, shift, days_worked);
    if (ub <= best_objective_) {
      ++nodes_pruned_;
      return;
    }

    bool last_shift = (shift + 1 == num_shifts_);
    int  next_day   = last_shift ? day + 1 : day;
    int  next_shift = last_shift ? 0       : shift + 1;

    // Empleados libres hoy
    std::vector<int> available;
    for (int emp = 0; emp < num_employees_; ++emp)
      if (!worked_today[emp]) available.push_back(emp);

    int required    = static_cast<int>(minimum_[day][shift]);
    auto candidates = Combinations(available, required, day, shift);

    for (auto& assignment : candidates) {
      std::vector<int> newly_working;
      double delta_sat = 0.0;
      for (const auto& [emp_id, sat] : assignment) {
        if (!worked_today[emp_id]) {
          worked_today[emp_id] = true;
          days_worked[emp_id]++;
          newly_working.push_back(emp_id);
        }
        delta_sat += sat;
      }
      bool covered   = (static_cast<int>(assignment.size()) >= required);
      double delta   = delta_sat + (covered ? 100.0 : 0.0);

      // Poda 1: factibilidad de descansos
      bool feasible = true;
      for (int emp : newly_working) {
        if (days_worked[emp] > max_work_days_[emp]) { feasible = false; break; }
      }

      if (feasible) {
        schedule[day][shift] = assignment;
        std::vector<bool> next_worked = worked_today;
        if (last_shift) next_worked.assign(num_employees_, false);

        BnB(next_day, next_shift, current_obj + delta,
            schedule, days_worked, next_worked);
      } else {
        ++nodes_pruned_;
      }
      // Deshacer
      schedule[day][shift].clear();
      for (int emp : newly_working) {
        worked_today[emp] = false;
        --days_worked[emp];
      }
    }
  }
};

inline std::unique_ptr<Solution> EmployeesBranchAndBound::Solve(const Instance* instance) {
  const auto* emp = dynamic_cast<const EmployeesInstance*>(instance);
  if (!emp) return nullptr;

  satisfaction_  = emp->GetSatisfactionRates();
  minimum_       = emp->GetRequiredEmployees();
  days_off_      = emp->GetDaysOff();
  num_employees_ = static_cast<int>(days_off_.size());
  num_days_      = static_cast<int>(minimum_.size());
  num_shifts_    = static_cast<int>(minimum_[0].size());

  max_work_days_.resize(num_employees_);
  for (int emp = 0; emp < num_employees_; ++emp)
    max_work_days_[emp] = num_days_ - static_cast<int>(days_off_[emp]);

  std::cout << "[B&B] Empleados=" << num_employees_
            << "  Días="          << num_days_
            << "  Turnos="        << num_shifts_ << "\n"
            << "[B&B] Iniciando Branch & Bound con cota admisible...\n";

  FullSchedule    schedule(num_days_, DaySchedule(num_shifts_));
  std::vector<int>  days_worked(num_employees_, 0);
  std::vector<bool> worked_today(num_employees_, false);

  BnB(0, 0, 0.0, schedule, days_worked, worked_today);

  std::cout << "[B&B] Terminado.\n"
            << "[B&B] Nodos evaluados : " << nodes_visited_ << "\n"
            << "[B&B] Nodos podados   : " << nodes_pruned_  << "\n"
            << "[B&B] Mejor f(x)      : " << best_objective_ << "\n";

  return std::make_unique<EmployeesSolution>(best_schedule_, days_off_, num_days_);
}

#endif // EMPLOYEES_BRANCH_AND_BOUND_H