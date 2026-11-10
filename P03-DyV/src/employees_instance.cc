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

#include "employees_instance.h"

#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>
#include <stdexcept>
#include <iterator>


EmployeesInstance::EmployeesInstance(const std::string& json_filename) {
  std::ifstream file(json_filename);
  if (!file.is_open()) {
    throw std::runtime_error("Error: No se pudo abrir el archivo " + json_filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string json = buffer.str();
  
  std::smatch match;
  std::regex horizon_regex(R"("planningHorizon"\s*:\s*(\d+))");
  int num_days = 0;
  if (std::regex_search(json, match, horizon_regex)) {
    num_days = std::stoi(match[1].str());
  }

  std::regex employee_regex(R"("freeDays"\s*:\s*(\d+))");
  for (auto it = std::sregex_iterator(json.begin(), json.end(), employee_regex); 
    it != std::sregex_iterator(); ++it) {
    const int free_days = std::stoi((*it)[1].str());
    days_off_.push_back(free_days);
  }
  int num_employees = days_off_.size();

  std::regex shift_def_regex(R"("T\d+")");
  int num_shifts = std::distance(
    std::sregex_iterator(json.begin(), json.end(), shift_def_regex),
    std::sregex_iterator()
  );

  required_employees_.assign(num_days, std::vector<unsigned int>(num_shifts, 0));
  satisfaction_rates_.assign(num_employees, 
      std::vector<std::vector<int>>(num_days, std::vector<int>(num_shifts, 0)));

  std::regex sat_regex(R"("shift"\s*:\s*(\d+)\s*,\s*"employee"\s*:\s*(\d+)\s*,\s*"day"\s*:\s*(\d+)\s*,\s*"value"\s*:\s*(\d+))");
  for (auto it = std::sregex_iterator(json.begin(), json.end(), sat_regex); 
    it != std::sregex_iterator(); ++it) {
    int shift = std::stoi((*it)[1].str());
    int employee = std::stoi((*it)[2].str());
    int day = std::stoi((*it)[3].str());
    int value = std::stoi((*it)[4].str());
    satisfaction_rates_[employee][day][shift] = value;
  }

  std::regex req_regex(R"("shift"\s*:\s*(\d+)\s*,\s*"day"\s*:\s*(\d+)\s*,\s*"value"\s*:\s*(\d+))");
  for (auto it = std::sregex_iterator(json.begin(), json.end(), req_regex);
    it != std::sregex_iterator(); ++it) {
    int shift = std::stoi((*it)[1].str());
    int day = std::stoi((*it)[2].str());
    int value = std::stoi((*it)[3].str());
    required_employees_[day][shift] = value;
  }

  // // Para DEBUG: Imprimir los datos leídos
  // std::cout << "Número de días: " << num_days << std::endl;
  // std::cout << "Número de empleados: " << num_employees << std::endl;
  // std::cout << "Número de turnos: " << num_shifts << std::endl;
  // std::cout << "Días libres por empleado: ";
  // for (const auto& days : days_off_) {
  //   std::cout << days << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "Requisitos mínimos por día y turno:" << std::endl;
  // for (size_t day = 0; day < required_employees_.size(); ++day) {
  //   std::cout << "Día " << day << ": ";
  //   for (size_t shift = 0; shift < required_employees_[day].size(); ++shift) {
  //     std::cout << "Turno " << shift << ": " << required_employees_[day][shift] << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << "Satisfacción de empleados por día y turno:" << std::endl;
  // for (size_t employee = 0; employee < satisfaction_rates_.size(); ++employee) {
  //   std::cout << "Empleado " << employee << ":" << std::endl;
  //   for (size_t day = 0; day < satisfaction_rates_[employee].size(); ++day) {
  //     std::cout << "  Día " << day << ": ";
  //     for (size_t shift = 0; shift < satisfaction_rates_[employee][day].size(); ++shift) {
  //       std::cout << "Turno " << shift << ": " << satisfaction_rates_[employee][day][shift] << " ";
  //     }
  //     std::cout << std::endl;
  //   }
  // }
}