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

  int num_days = 0;
  std::smatch match;
  std::regex horizon_regex(R"("planningHorizon"\s*:\s*(\d+))");
  if (std::regex_search(json, match, horizon_regex)) {
    num_days = std::stoi(match[1].str());
  }

  std::regex employee_regex(R"("freeDays"\s*:\s*(\d+))");
  for (auto it = std::sregex_iterator(json.begin(), json.end(), employee_regex);
       it != std::sregex_iterator(); ++it) {
    days_off_.push_back(std::stoul((*it)[1].str()));
  }
  int num_employees = static_cast<int>(days_off_.size());

  int num_shifts = 0;
  std::regex shifts_array_regex(R"("shifts"\s*:\s*\[([^\]]*)\])");
  if (std::regex_search(json, match, shifts_array_regex)) {
    std::string shifts_content = match[1].str();
    std::regex shift_entry(R"("T\d+")");
    num_shifts = static_cast<int>(std::distance(
      std::sregex_iterator(shifts_content.begin(), shifts_content.end(), shift_entry),
      std::sregex_iterator()));
  }
  if (num_shifts == 0) {
    std::regex shift_def_regex(R"("T\d+")");
    num_shifts = static_cast<int>(std::distance(
      std::sregex_iterator(json.begin(), json.end(), shift_def_regex),
      std::sregex_iterator()));
  }

  required_employees_.assign(num_days,
      std::vector<unsigned int>(num_shifts, 0));
  satisfaction_rates_.assign(num_employees,
      std::vector<std::vector<int>>(num_days,
          std::vector<int>(num_shifts, 0)));

  std::regex sat_regex(
    R"("shift"\s*:\s*(\d+)\s*,\s*"employee"\s*:\s*(\d+)\s*,\s*"day"\s*:\s*(\d+)\s*,\s*"value"\s*:\s*(\d+))");
  for (auto it = std::sregex_iterator(json.begin(), json.end(), sat_regex);
       it != std::sregex_iterator(); ++it) {
    int shift    = std::stoi((*it)[1].str());
    int employee = std::stoi((*it)[2].str());
    int day      = std::stoi((*it)[3].str());
    int value    = std::stoi((*it)[4].str());
    if (employee < num_employees && day < num_days && shift < num_shifts) {
      satisfaction_rates_[employee][day][shift] = value;
    }
  }

  std::regex req_regex(
    R"("shift"\s*:\s*(\d+)\s*,\s*"day"\s*:\s*(\d+)\s*,\s*"value"\s*:\s*(\d+))");
  for (auto it = std::sregex_iterator(json.begin(), json.end(), req_regex);
       it != std::sregex_iterator(); ++it) {
    int shift = std::stoi((*it)[1].str());
    int day   = std::stoi((*it)[2].str());
    int value = std::stoi((*it)[3].str());
    if (day < num_days && shift < num_shifts) {
      required_employees_[day][shift] = static_cast<unsigned int>(value);
    }
  }
  // std::cout << "Número de días      : " << num_days      << "\n"
  //           << "Número de empleados : " << num_employees << "\n"
  //           << "Número de turnos    : " << num_shifts     << "\n"
  //           << "Días libres         : ";
  // for (auto d : days_off_) std::cout << d << " ";
  // std::cout << "\n";
}