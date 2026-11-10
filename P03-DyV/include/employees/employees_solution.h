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

#ifndef EMPLOYEES_SOLUTION_H
#define EMPLOYEES_SOLUTION_H

#include "solution.h"
#include <vector>

using Shift = std::vector<std::pair<int, int>>; 
using Day = std::vector<Shift>;                 
using ScheduleMatrix = std::vector<Day>;        

class EmployeesSolution : public Solution {
 public:
  EmployeesSolution(const ScheduleMatrix& schedule,
                    const std::vector<unsigned int>& free_days,
                    int total_days)
      : schedule_(schedule), 
        free_days_(free_days), 
        total_days_(total_days) {}

  const ScheduleMatrix& GetSchedule() const { return schedule_; }

  double ObjectiveFunction() const;
  bool isFeasible() const override;
 private:
  ScheduleMatrix schedule_;
  std::vector<unsigned int> free_days_;
  int total_days_;
};

#endif // EMPLOYEES_SOLUTION_H