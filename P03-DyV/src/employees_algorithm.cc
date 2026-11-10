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

#include "employees_algorithm.h"
#include <algorithm>
#include <functional>

/**
 * @brief Condición de parada. El problema es lo suficientemente pequeño 
 * si la instancia representa un único día a planificar.
 */
bool EmployeesAlgorithm::Small(const Instance* instance) {
  const EmployeesInstance* emp_inst = dynamic_cast<const EmployeesInstance*>(instance);
  return emp_inst->GetRequiredEmployees().size() <= 1;
}

/**
 * @brief Resuelve el caso base (1 día) usando un enfoque voraz.
 */
std::unique_ptr<Solution> EmployeesAlgorithm::SolveSmall(const Instance* instance) {
  const EmployeesInstance* emp_inst = dynamic_cast<const EmployeesInstance*>(instance);
  auto satisfaction = emp_inst->GetSatisfactionRates();
  auto required_employees = emp_inst->GetRequiredEmployees();

  int num_employees = satisfaction.size();
  int num_shifts = required_employees[0].size();

  ScheduleMatrix schedule(1, Day(num_shifts)); 
  std::vector<bool> worked_today(num_employees, false);
  for (int shift{0}; shift < num_shifts; ++shift) {
    int required = required_employees[0][shift];
    std::vector<std::pair<int, int>> candidates;

    for (int current_emp{0}; current_emp < num_employees; ++current_emp) {
      if (!worked_today[current_emp]) {
        candidates.push_back({satisfaction[current_emp][0][shift], current_emp});
      }
    }

    std::sort(candidates.begin(), candidates.end(), std::greater<std::pair<int, int>>());

    for (int i{0}; i < required && i < candidates.size(); ++i) {
      int emp_id = candidates[i].second;
      int satisfaction = candidates[i].first;
      schedule[0][shift].push_back({emp_id, satisfaction});
      worked_today[emp_id] = true;
    }
  }
  return std::make_unique<EmployeesSolution>(schedule, free_days_, total_days_);
}

/**
 * @brief Divide la instancia binariamente partiendo los días a la mitad.
 * @return Un vector con dos subinstancias, cada una con la mitad de los días.
 */
std::vector<std::unique_ptr<Instance>> EmployeesAlgorithm::Divide(const Instance* instance) {
  const EmployeesInstance* emp_inst = dynamic_cast<const EmployeesInstance*>(instance);
  
  auto required_employees = emp_inst->GetRequiredEmployees();
  int total_days = required_employees.size();
  int mid = total_days / 2;
  RequiredMatrix required_left(required_employees.begin(), required_employees.begin() + mid);
  RequiredMatrix required_right(required_employees.begin() + mid, required_employees.end());
  
  auto satisfaction = emp_inst->GetSatisfactionRates();
  int num_employees = satisfaction.size();
  SatisfactionMatrix satisfaction_left(num_employees);
  SatisfactionMatrix satisfaction_right(num_employees);
  for (int current_emp{0}; current_emp < num_employees; ++current_emp) {
    satisfaction_left[current_emp] = std::vector<std::vector<int>>(satisfaction[current_emp].begin(), satisfaction[current_emp].begin() + mid);
    satisfaction_right[current_emp] = std::vector<std::vector<int>>(satisfaction[current_emp].begin() + mid, satisfaction[current_emp].end());
  }
  
  auto days_off = emp_inst->GetDaysOff();
  std::vector<std::unique_ptr<Instance>> subproblems;
  subproblems.push_back(std::make_unique<EmployeesInstance>(satisfaction_left, required_left, days_off));
  subproblems.push_back(std::make_unique<EmployeesInstance>(satisfaction_right, required_right, days_off));
  return subproblems;
}

// #include <iostream>

/**
 * @brief Mezcla las soluciones parciales y aplica heurísticas globales
 * para garantizar que se cumplan los días de descanso obligatorios.
 */
std::unique_ptr<Solution> EmployeesAlgorithm::Combine(std::vector<std::unique_ptr<Solution>> partial_solutions) {
  auto* solution_left = dynamic_cast<EmployeesSolution*>(partial_solutions[0].get());
  auto* solution_right = dynamic_cast<EmployeesSolution*>(partial_solutions[1].get());
  
  ScheduleMatrix schedule = solution_left->GetSchedule();
  ScheduleMatrix right_schedule = solution_right->GetSchedule();
  schedule.insert(schedule.end(), right_schedule.begin(), right_schedule.end());
  
  int num_days = schedule.size();
  int num_employees = free_days_.size();
  
  std::vector<int> days_worked(num_employees, 0);
  for (const auto& day : schedule)
    for (const auto& shift : day)
      for (const auto& [emp_id, satisfaction] : shift)
        ++days_worked[emp_id];
  
  for (int current_emp{0}; current_emp < num_employees; ++current_emp) {
    int max_allowed_work_days = total_days_ - free_days_[current_emp];
    
    while (days_worked[current_emp] > max_allowed_work_days) {
      int minimum_satisfaction = 99999999;
      int target_day = -1, target_shift = -1, target_idx = -1;
      
      for (int day{0}; day < num_days; ++day) {
        for (int shift{0}; shift < schedule[day].size(); ++shift) {
          for (int idx{0}; idx < schedule[day][shift].size(); ++idx) {
            const auto& [emp_id, satisfaction] = schedule[day][shift][idx];

            if (emp_id == current_emp && satisfaction < minimum_satisfaction) {
              minimum_satisfaction = satisfaction;
              target_day = day;
              target_shift = shift;
              target_idx = idx;
            }
          }
        }
      }
      if (target_day != -1) {
        schedule[target_day][target_shift].erase(schedule[target_day][target_shift].begin() + target_idx);
        --days_worked[current_emp];
        
        std::vector<bool> is_working_today(num_employees, false);
        for (const auto& shift_list : schedule[target_day]) {
          for (const auto& [emp_id, satisfaction] : shift_list) {
            is_working_today[emp_id] = true;
          }
        }

        int best_candidate = -1;
        int best_satisfaction = -1;

        for (int candidate = 0; candidate < num_employees; ++candidate) {
          int max_candidate_days = total_days_ - free_days_[candidate];
          
          if (!is_working_today[candidate] && days_worked[candidate] < max_candidate_days) {
            int current_sat = satisfaction_rates_[candidate][target_day][target_shift];
            
            if (current_sat > best_satisfaction) {
              best_satisfaction = current_sat;
              best_candidate = candidate;
            }
          }
        }
        if (best_candidate != -1) {
          schedule[target_day][target_shift].push_back({best_candidate, best_satisfaction});
          ++days_worked[best_candidate];
        }

        // std::cout << "Reasignando día " << target_day << ", turno " << target_shift 
        //           << ": empleado " << current_emp << " (satisfacción " << minimum_satisfaction 
        //           << ") reemplazado por empleado " << best_candidate 
        //           << " (satisfacción " << best_satisfaction << ")" << std::endl;
      } else {
        break; // Seguro anti bucles infinitos
      }
    }
  }
  return std::make_unique<EmployeesSolution>(schedule, free_days_, total_days_);
}