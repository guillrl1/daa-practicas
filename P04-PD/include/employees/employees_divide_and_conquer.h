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

#ifndef EMPLOYEES_DIVIDE_AND_CONQUER_H
#define EMPLOYEES_DIVIDE_AND_CONQUER_H

#include "divide_and_conquer.h"
#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"
#include "employees_combine_solver.h"

#include <vector>
#include <memory>
#include <algorithm>

/**
 * @class EmployeesDivideAndConquer
 * @brief Algoritmo DyV para planificación de empleados con SmallSolver
 *        inyectable. Sustituye a EmployeesAlgorithm de la práctica 3.
 */
class EmployeesDivideAndConquer : public DivideAndConquer {
 public:
  /**
   * @param small_solver Algoritmo que resuelve instancias de 1 día. Puede ser Greedy o 
   *     DynamicProgramming (o incluso otro DyV, como en divide_divide_and_greedy.json).
   * @param days_off Días de descanso globales de cada empleado.
   * @param horizon Horizonte total de planificación.
   */
  EmployeesDivideAndConquer(std::unique_ptr<Algorithm> small_solver,
                            std::unique_ptr<CombineSolver> combine_solver,
                            const std::vector<unsigned int>& days_off,
                            int horizon)
      : small_solver_(std::move(small_solver)), 
        combine_solver_(std::move(combine_solver)),
        global_days_off_(days_off),
        global_horizon_(horizon) {}

 private:
  std::unique_ptr<Algorithm> small_solver_;
  std::unique_ptr<CombineSolver> combine_solver_;
  std::vector<unsigned int> global_days_off_;
  int global_horizon_;

  /**
   * @brief Condición de parada: instancia de 1 solo día.
   */
  bool Small(const Instance* instance) override {
    const auto* emp = dynamic_cast<const EmployeesInstance*>(instance);
    return emp->GetRequiredEmployees().size() <= 1;
  }

  /**
   * @brief Delega en el SmallSolver inyectado (Greedy o DP).
   */
  std::unique_ptr<Solution> SolveSmall(const Instance* instance) override {
    return small_solver_->Solve(instance);
  }


  /**
   * @brief División binaria: partir los días por la mitad.
   *        Los days_off se pasan íntegros a ambas subinstancias (son globales).
   */
  std::vector<std::unique_ptr<Instance>> Divide(const Instance* instance) override {
    const auto* emp = dynamic_cast<const EmployeesInstance*>(instance);
    auto satisfaction = emp->GetSatisfactionRates();
    auto minimum = emp->GetRequiredEmployees();
    auto days_off  = emp->GetDaysOff();

    int total = static_cast<int>(minimum.size());
    int mid = total / 2;

    std::vector<std::vector<unsigned int>> min_left(minimum.begin(), minimum.begin() + mid);
    std::vector<std::vector<unsigned int>> min_right(minimum.begin() + mid, minimum.end());

    int satisfaction_size = static_cast<int>(satisfaction.size());
    std::vector<std::vector<std::vector<int>>> sat_left(satisfaction_size), sat_right(satisfaction_size);
    for (int emp = 0; emp < satisfaction_size; ++emp) {
      sat_left[emp]  = std::vector<std::vector<int>>(satisfaction[emp].begin(), satisfaction[emp].begin() + mid);
      sat_right[emp] = std::vector<std::vector<int>>(satisfaction[emp].begin() + mid, satisfaction[emp].end());
    }

    std::vector<std::unique_ptr<Instance>> subs;
    subs.push_back(std::make_unique<EmployeesInstance>(sat_left,  min_left,  days_off));
    subs.push_back(std::make_unique<EmployeesInstance>(sat_right, min_right, days_off));
    return subs;
  }

  /**
   * @brief Combina dos horarios parciales y aplica el ajuste de descansos.
   */
  std::unique_ptr<Solution> Combine(
      std::vector<std::unique_ptr<Solution>> partialSolutions) override {
    auto left_sol = dynamic_cast<EmployeesSolution*>(partialSolutions[0].get());
    auto right_sol = dynamic_cast<EmployeesSolution*>(partialSolutions[1].get());

    // Combinar las soluciones parciales usando el CombineSolver inyectado
    EmployeesCombineSolver combine_solver;
    return combine_solver.Combine(left_sol, right_sol, global_days_off_, global_horizon_);
  }
};

#endif // EMPLOYEES_DIVIDE_AND_CONQUER_H