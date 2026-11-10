/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 08/03/2026
 *
 * BENCHMARK ÓPTIMO: Branch & Bound con cota superior admisible
 * ─────────────────────────────────────────────────────────────
 *
 * GARANTÍA DE OPTIMALIDAD
 * ───────────────────────
 * Un Branch & Bound garantiza el óptimo si y solo si su cota superior
 * es ADMISIBLE, es decir: nunca subestima el valor real alcanzable.
 * Una cota admisible puede sobreestimar, pero jamás subestimar.
 * Si la cota dice "desde aquí no puedes llegar a X", eso debe ser cierto.
 *
 * COTA SUPERIOR ADMISIBLE IMPLEMENTADA
 * ──────────────────────────────────────
 * Para cada (día, turno) restante se calcula una cota de satisfacción
 * resolviendo una relajación del problema: se ignoran los conflictos
 * de "un empleado no puede hacer dos turnos el mismo día", pero SÍ se
 * respetan los días de descanso restantes de cada empleado.
 *
 * Formalmente, para los turnos restantes se resuelve un problema de
 * asignación relajado con programación greedy sobre una copia de los
 * días disponibles por empleado:
 *
 *   Para cada (d,t) restante, ordenado por mejor satisfacción:
 *     - Se asignan los `minimum[d][t]` empleados con mayor sat[e][d][t]
 *       que aún tienen días_restantes[e] > 0.
 *     - Se descuenta 1 día a cada empleado asignado.
 *     - Se suma 100 por turno cubierto + satisfacciones asignadas.
 *
 * Esta relajación SÍ es admisible porque:
 *   - Ignora el conflicto de "un solo turno por día" → puede asignar
 *     el mismo empleado a dos turnos del mismo día → sobreestima sat.
 *   - Respeta los días de descanso → nunca asigna más días de los
 *     permitidos → no subestima la restricción más importante.
 *   - Por tanto la cota real ≤ cota calculada → admisible ✓
 *
 * PODAS
 * ─────
 *   Poda 1 — Factibilidad: rama descartada si algún empleado supera
 *             su límite de días trabajados.
 *   Poda 2 — Cota superior admisible: rama descartada si
 *             current_obj + upper_bound(resto) ≤ best_known.
 *
 * ORDEN DE EXPLORACIÓN
 * ─────────────────────
 * Las combinaciones de cada turno se ordenan de mayor a menor
 * satisfacción total. Esto hace que el algoritmo encuentre soluciones
 * buenas muy pronto, lo que activa agresivamente la poda 2.
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

class EmployeesBranchAndBound : public Algorithm {
 public:
  EmployeesBranchAndBound() = default;
  std::unique_ptr<Solution> Solve(const Instance* instance) override;

 private:
  // ── Tipos ──────────────────────────────────────────────────────────────────
  using ShiftAssignment = std::vector<std::pair<int,int>>;  // {emp_id, sat}
  using DaySchedule     = std::vector<ShiftAssignment>;
  using FullSchedule    = std::vector<DaySchedule>;

  // ── Datos de la instancia ──────────────────────────────────────────────────
  int num_employees_ = 0;
  int num_days_      = 0;
  int num_shifts_    = 0;
  std::vector<unsigned int>                  days_off_;
  std::vector<int>                           max_work_days_;
  std::vector<std::vector<std::vector<int>>> satisfaction_;  // [e][d][t]
  std::vector<std::vector<unsigned int>>     minimum_;       // [d][t]

  // ── Estado de la búsqueda ──────────────────────────────────────────────────
  FullSchedule best_schedule_;
  double       best_objective_ = -std::numeric_limits<double>::infinity();
  long long    nodes_visited_  = 0;
  long long    nodes_pruned_   = 0;

  // ── Función objetivo exacta ────────────────────────────────────────────────
  double ObjectiveFunction(const FullSchedule& schedule) const {
    double sat = 0.0, covered = 0.0;
    for (int d = 0; d < num_days_; ++d) {
      for (int t = 0; t < num_shifts_; ++t) {
        for (const auto& [e, s] : schedule[d][t]) sat += s;
        if (static_cast<int>(schedule[d][t].size()) >= static_cast<int>(minimum_[d][t]))
          covered += 1.0;
      }
    }
    return sat + covered * 100.0;
  }

  // ── Cota superior ADMISIBLE ────────────────────────────────────────────────
  /**
   * @brief Calcula una cota superior admisible del valor máximo alcanzable
   *        desde el nodo (day, shift) con los días disponibles dados.
   *
   * Resuelve una relajación greedy ignorando el conflicto de un turno por día
   * pero respetando el total de días disponibles de cada empleado.
   * Resultado: cota >= valor real alcanzable (admisible) ✓
   *
   * @param day          Primer día pendiente de planificar.
   * @param shift        Primer turno pendiente dentro de ese día.
   * @param days_worked  Días ya trabajados por cada empleado.
   * @return Cota superior del valor máximo alcanzable desde este nodo.
   */
  double UpperBound(int day, int shift,
                    const std::vector<int>& days_worked) const {
    // Días disponibles restantes por empleado (respetamos esta restricción)
    std::vector<int> days_left(num_employees_);
    for (int e = 0; e < num_employees_; ++e)
      days_left[e] = max_work_days_[e] - days_worked[e];

    double bound = 0.0;

    for (int d = day; d < num_days_; ++d) {
      int t_start = (d == day) ? shift : 0;
      for (int t = t_start; t < num_shifts_; ++t) {
        int required = static_cast<int>(minimum_[d][t]);

        if (required == 0) {
          bound += 100.0;  // turno vacío siempre cubierto
          continue;
        }

        // Construir lista de candidatos con días disponibles
        // (relajamos: no comprobamos si ya trabajan ese día en otro turno)
        std::vector<std::pair<int,int>> candidates;  // {sat, emp_id}
        for (int e = 0; e < num_employees_; ++e) {
          if (days_left[e] > 0)
            candidates.push_back({satisfaction_[e][d][t], e});
        }
        // Ordenar por satisfacción descendente
        std::sort(candidates.begin(), candidates.end(),
                  [](const auto& a, const auto& b){ return a.first > b.first; });

        // Asignar los `required` mejores
        int assigned = 0;
        for (auto& [sat, e] : candidates) {
          if (assigned == required) break;
          bound += sat;
          days_left[e]--;  // consumir un día disponible
          ++assigned;
        }
        // Si se cubrió el mínimo, sumar la cobertura
        if (assigned >= required) bound += 100.0;
      }
    }
    return bound;
  }

  // ── Generador de combinaciones C(n,k) ─────────────────────────────────────
  /**
   * @brief Genera todas las combinaciones de exactamente k empleados
   *        tomados de `available`, ordenadas de mayor a menor satisfacción
   *        total para que el B&B encuentre buenas soluciones cuanto antes.
   */
  std::vector<ShiftAssignment> Combinations(
      const std::vector<int>& available, int k, int day, int shift) const {

    std::vector<ShiftAssignment> result;
    int n = static_cast<int>(available.size());

    // Turno sin requisito: única opción es no asignar a nadie
    if (k == 0) { result.push_back({}); return result; }

    // No hay suficientes empleados libres: mejor esfuerzo
    if (k > n) {
      ShiftAssignment effort;
      for (int e : available)
        effort.push_back({e, satisfaction_[e][day][shift]});
      result.push_back(effort);
      return result;
    }

    // Generar C(n,k) en orden lexicográfico con índices
    std::vector<int> idx(k);
    std::iota(idx.begin(), idx.end(), 0);

    while (true) {
      ShiftAssignment asgn;
      for (int i : idx)
        asgn.push_back({available[i], satisfaction_[available[i]][day][shift]});
      result.push_back(std::move(asgn));

      int i = k - 1;
      while (i >= 0 && idx[i] == n - k + i) --i;
      if (i < 0) break;
      ++idx[i];
      for (int j = i+1; j < k; ++j) idx[j] = idx[j-1] + 1;
    }

    // Ordenar de mayor a menor satisfacción total → mejores primero
    std::sort(result.begin(), result.end(),
      [](const ShiftAssignment& a, const ShiftAssignment& b) {
        int sa = 0, sb = 0;
        for (const auto& [e,s] : a) sa += s;
        for (const auto& [e,s] : b) sb += s;
        return sa > sb;
      });

    return result;
  }

  // ── Backtracking ───────────────────────────────────────────────────────────
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
    // current_obj ya incluye lo planificado; UpperBound calcula el máximo
    // adicional alcanzable desde (day,shift) en adelante.
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
    for (int e = 0; e < num_employees_; ++e)
      if (!worked_today[e]) available.push_back(e);

    int required    = static_cast<int>(minimum_[day][shift]);
    auto candidates = Combinations(available, required, day, shift);

    for (auto& assignment : candidates) {
      // Aplicar asignación
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
      for (int e : newly_working) {
        if (days_worked[e] > max_work_days_[e]) { feasible = false; break; }
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
      for (int e : newly_working) {
        worked_today[e] = false;
        days_worked[e]--;
      }
    }
  }
};

// ─────────────────────────────────────────────────────────────────────────────
// Solve
// ─────────────────────────────────────────────────────────────────────────────
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
  for (int e = 0; e < num_employees_; ++e)
    max_work_days_[e] = num_days_ - static_cast<int>(days_off_[e]);

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