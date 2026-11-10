/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 4: Experimentación con Programación Dinámica y Patrón Factory
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 17/03/2026
 *
 * Interfaz para el algoritmo de combinación inyectable en DyV.
 */

#ifndef EMPLOYEES_COMBINE_SOLVER_H
#define EMPLOYEES_COMBINE_SOLVER_H

#include "employees_solution.h"
#include "employees_instance.h"
#include <memory>
#include <vector>

/**
 * @class CombineSolver
 * @brief Interfaz que representa el algoritmo de combinación del DyV.
 *        Permite sustituir el Combine sin modificar EmployeesDivideAndConquer.
 */
class CombineSolver {
 public:
  virtual ~CombineSolver() = default;

  /**
   * @brief Combina dos soluciones parciales en una solución completa.
   * @param left            Solución de la primera mitad.
   * @param right           Solución de la segunda mitad.
   * @param global_days_off Días de descanso globales por empleado.
   * @param global_horizon  Horizonte total de planificación.
   * @return Solución combinada y ajustada.
   */
  virtual std::unique_ptr<EmployeesSolution> Combine(
      const EmployeesSolution* left,
      const EmployeesSolution* right,
      const std::vector<unsigned int>& global_days_off,
      int global_horizon) = 0;
};

class EmployeesCombineSolver : public CombineSolver {
 public:
  std::unique_ptr<EmployeesSolution> Combine(
      const EmployeesSolution* left,
      const EmployeesSolution* right,
      const std::vector<unsigned int>& global_days_off,
      int global_horizon) override {

    auto* left_sol  = dynamic_cast<EmployeesSolution*>(const_cast<EmployeesSolution*>(left));
    auto* right_sol = dynamic_cast<EmployeesSolution*>(const_cast<EmployeesSolution*>(right));

    auto schedule = left_sol->GetSchedule();
    const auto& right_sched = right_sol->GetSchedule();
    schedule.insert(schedule.end(), right_sched.begin(), right_sched.end());

    int total_size = static_cast<int>(schedule.size());
    int total_global_days_off = static_cast<int>(global_days_off.size());

    // Contar días trabajados
    std::vector<int> days_worked(total_global_days_off, 0);
    for (const auto& day : schedule)
      for (const auto& shift : day)
        for (const auto& [emp_id, sat] : shift)
          days_worked[emp_id]++;

    // Ajuste: eliminar turnos de menor satisfacción si se supera el límite
    for (int emp = 0; emp < total_global_days_off; ++emp) {
      int max_allowed = global_horizon - static_cast<int>(global_days_off[emp]);
      while (days_worked[emp] > max_allowed) {
        int min_sat = std::numeric_limits<int>::max();
        int td = -1, ts = -1, ti = -1;
        for (int d = 0; d < total_size; ++d)
          for (int s = 0; s < static_cast<int>(schedule[d].size()); ++s)
            for (int i = 0; i < static_cast<int>(schedule[d][s].size()); ++i)
              if (schedule[d][s][i].first == emp &&
                  schedule[d][s][i].second < min_sat) {
                min_sat = schedule[d][s][i].second;
                td = d; ts = s; ti = i;
              }
        if (td == -1) break;
        schedule[td][ts].erase(schedule[td][ts].begin() + ti);
        --days_worked[emp];
      }
    }
    return std::make_unique<EmployeesSolution>(schedule, global_days_off, global_horizon);
  }
};

class EmployeesCombineSolver2 : public CombineSolver {
 public:
  std::unique_ptr<EmployeesSolution> Combine(
      const EmployeesSolution* left,
      const EmployeesSolution* right,
      const std::vector<unsigned int>& global_days_off,
      int global_horizon) override {

    auto* left_sol  = dynamic_cast<EmployeesSolution*>(const_cast<EmployeesSolution*>(left));
    auto* right_sol = dynamic_cast<EmployeesSolution*>(const_cast<EmployeesSolution*>(right));

    auto schedule = left_sol->GetSchedule();
    const auto& right_sched = right_sol->GetSchedule();
    schedule.insert(schedule.end(), right_sched.begin(), right_sched.end());

    int total_size = static_cast<int>(schedule.size());
    int total_global_days_off = static_cast<int>(global_days_off.size());

    // Contar días trabajados
    std::vector<int> days_worked(total_global_days_off, 0);
    for (const auto& day : schedule)
      for (const auto& shift : day)
        for (const auto& [emp_id, sat] : shift)
          days_worked[emp_id]++;

    // Ajuste: eliminar turnos de menor satisfacción si se supera el límite
    for (int emp = 0; emp < total_global_days_off; ++emp) {
      int max_allowed = global_horizon - static_cast<int>(global_days_off[emp]);
      while (days_worked[emp] > max_allowed) {
        int min_sat = std::numeric_limits<int>::max();
        int td = -1, ts = -1, ti = -1;
        for (int d = 0; d < total_size; ++d)
          for (int s = 0; s < static_cast<int>(schedule[d].size()); ++s)
            for (int i = 0; i < static_cast<int>(schedule[d][s].size()); ++i)
              if (schedule[d][s][i].first == emp &&
                  schedule[d][s][i].second < min_sat) {
                min_sat = schedule[d][s][i].second;
                td = d; ts = s; ti = i;
              }
        if (td == -1) break;
        schedule[td][ts].erase(schedule[td][ts].begin() + ti);
        --days_worked[emp];
      }
    }
    // Eliminar un empleado aleatorio de un dia aleatorio
        for (int emp = 0; emp < total_global_days_off; ++emp) {
        int max_allowed = global_horizon - static_cast<int>(global_days_off[emp]);
        if (days_worked[emp] > max_allowed) {
            for (int d = 0; d < total_size; ++d)
            for (int s = 0; s < static_cast<int>(schedule[d].size()); ++s)
                for (int i = 0; i < static_cast<int>(schedule[d][s].size()); ++i)
                if (schedule[d][s][i].first == emp) {
                    schedule[d][s].erase(schedule[d][s].begin() + i);
                    --days_worked[emp];
                    break;
                }
        }
        }
    return std::make_unique<EmployeesSolution>(schedule, global_days_off, global_horizon);
  }
};
#endif // EMPLOYEES_COMBINE_SOLVER_H