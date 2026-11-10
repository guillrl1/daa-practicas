/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 1/03/2026
 */

#include "employees_solution.h"

/**
 * @brief Devuelve el valor de la función objetivo para la solución de los 
 * empleados, que se calcula como la suma de la satisfacción de los empleados 
 * y un puntaje adicional por cada turno cubierto.
 * @return El valor de la función objetivo para la solución de los empleados.
 */
double EmployeesSolution::ObjectiveFunction() const {
  double satisfaction_score = 0.0;
  for (const auto& day : schedule_)
    for (const auto& shift : day)
      for (const auto& [employee_id, satisfaction] : shift)
        satisfaction_score += satisfaction;

  double coverage_score = 0.0;
  for (const auto& day : schedule_)
    for (const auto& shift : day)
      if (!shift.empty()) 
        ++coverage_score;

  const double coverage_weight = 100.0;
  return satisfaction_score + coverage_score * coverage_weight;
}


/**
 * @brief Verifica si la solución es factible garantizando los descansos.
 * @return true si la solución es factible, false de lo contrario.
 */
bool EmployeesSolution::isFeasible() const {
  std::vector<int> days_worked(free_days_.size(), 0);
  for (const auto& day : schedule_) {
    std::vector<bool> worked_today(free_days_.size(), false);
    for (const auto& shift : day) {
      for (const auto& [employee_id, satisfaction] : shift) {
        worked_today[employee_id] = true;
      }
    }
    for (size_t current_emp = 0; current_emp < worked_today.size(); ++current_emp) {
      if (worked_today[current_emp]) {
        days_worked[current_emp]++;
      }
    }
  }
  for (size_t current_emp = 0; current_emp < days_worked.size(); ++current_emp) {
    int max_allowed_work_days = total_days_ - free_days_[current_emp];
    if (days_worked[current_emp] > max_allowed_work_days) {
      return false;
    }
  }
  return true;
}