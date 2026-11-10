/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 1/03/2026
 */

#ifndef EMPLOYEES_INSTANCE_H
#define EMPLOYEES_INSTANCE_H

#include "instance.h"

#include <vector>
#include <string>

using SatisfactionMatrix = std::vector<std::vector<std::vector<int>>>;
using RequiredMatrix = std::vector<std::vector<unsigned int>>;

class EmployeesInstance : public Instance {
  
 public:
  EmployeesInstance() {}
  EmployeesInstance(const std::vector<std::vector<std::vector<int>>>& satisfaction_rates,
                    const std::vector<std::vector<unsigned int>>& required_employees,
                    const std::vector<unsigned int>& days_off)
      : satisfaction_rates_(satisfaction_rates),
        required_employees_(required_employees),
        days_off_(days_off) {}
  EmployeesInstance(const std::string& json_filename);
  std::vector<std::vector<std::vector<int>>> GetSatisfactionRates() const {
    return satisfaction_rates_;
  }

  std::vector<std::vector<unsigned int>> GetRequiredEmployees() const {
    return required_employees_;
  }
  
  std::vector<unsigned int> GetDaysOff() const {
    return days_off_;
  }
 private:
  std::vector<std::vector<std::vector<int>>>satisfaction_rates_;
  std::vector<std::vector<unsigned int>>required_employees_;
  std::vector<unsigned int>days_off_; 
};

#endif // EMPLOYEES_INSTANCE_H