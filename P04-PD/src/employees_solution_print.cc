/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alumno
 * Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 1/03/2026
 */

#include "employees_solution_print.h"

#include <iostream>
#include <fstream>
#include <iomanip> // Para formatear decimales si hiciera falta

/**
 * @brief Imprime la solución detallada en el flujo indicado (consola o archivo)
 */
void EmployeesSolutionPrint::Print(std::ostream& os) const {
  os << "HORARIO DE EMPLEADOS " << std::endl;
  
  const auto& schedule = solution_.GetSchedule();
  int total_satisfaction = 0;
  int total_assignments = 0;

  for (size_t day = 0; day < schedule.size(); ++day) {
    os << "Día " << day + 1 << ":" << std::endl;
    for (size_t shift = 0; shift < schedule[day].size(); ++shift) {
      os << "  Turno " << shift + 1 << ": ";

      if (schedule[day][shift].empty()) {
        os << "[Turno Descubierto]";
      } else {
        for (const auto& [employee_id, satisfaction] : schedule[day][shift]) {
          os << "Empleado " << employee_id << " (Sat: " << satisfaction << ")  ";
          total_satisfaction += satisfaction;
          total_assignments++;
        }
      }
      os << std::endl;
    }
    os << std::endl;
  }

  double average_satisfaction = (total_assignments > 0) ? static_cast<double>(total_satisfaction) / total_assignments : 0.0;

  os << "RESUMEN DE MÉTRICAS" << std::endl;
  os << "-> Satisfacción Total:    " << total_satisfaction << std::endl;
  os << "-> Satisfacción Promedio: " << average_satisfaction << std::endl;
  os << "-> Función Objetivo:      " << solution_.ObjectiveFunction() << std::endl;
  os << "-> Solución Factible:     " 
     << (solution_.isFeasible() ? "SÍ (Descansos cumplidos)" : "NO (Restricciones vulneradas)") << std::endl;
}

/**
 * @brief Imprime la solución detallada en formato JSON descriptivo y profesional
 */
void EmployeesSolutionPrint::PrintAsJSON(std::ostream& os) const {
  const auto& schedule = solution_.GetSchedule();
  int total_satisfaction = 0;
  int total_assignments = 0;

  os << "{" << std::endl;
  os << "  \"horario\": [" << std::endl;
  
  for (size_t day = 0; day < schedule.size(); ++day) {
    os << "    {" << std::endl;
    os << "      \"dia\": " << day + 1 << "," << std::endl;
    os << "      \"turnos\": [" << std::endl;
    
    for (size_t shift = 0; shift < schedule[day].size(); ++shift) {
      os << "        {" << std::endl;
      os << "          \"turno\": " << shift + 1 << "," << std::endl;
      os << "          \"empleados\": [";

      for (size_t i = 0; i < schedule[day][shift].size(); ++i) {
        int emp_id = schedule[day][shift][i].first;
        int sat = schedule[day][shift][i].second;
        
        total_satisfaction += sat;
        total_assignments++;

        os << "{\"empleado\": " << emp_id << ", \"satisfaccion\": " << sat << "}";
        
        if (i < schedule[day][shift].size() - 1) {
          os << ", ";
        }
      }
      os << "]" << std::endl;
      
      os << "        }";
      if (shift < schedule[day].size() - 1) {
        os << ",";
      }
      os << std::endl;
    }
    os << "      ]" << std::endl;
    
    os << "    }";
    if (day < schedule.size() - 1) {
      os << ",";
    }
    os << std::endl;
  }
  os << "  ]," << std::endl;

  double average_satisfaction = (total_assignments > 0) 
      ? static_cast<double>(total_satisfaction) / total_assignments 
      : 0.0;

  os << "  \"metricas\": {" << std::endl;
  os << "    \"satisfaccion_total\": " << total_satisfaction << "," << std::endl;
  os << "    \"satisfaccion_promedio\": " << average_satisfaction << "," << std::endl;
  os << "    \"funcion_objetivo\": " << solution_.ObjectiveFunction() << "," << std::endl;
  os << "    \"es_factible\": " << (solution_.isFeasible() ? "true" : "false") << std::endl;
  os << "  }" << std::endl;
  os << "}" << std::endl;
}

/**
 * @brief Guarda la salida normal en un archivo de texto
 */
void EmployeesSolutionPrint::SaveToFile(const std::string& filename) const {
  std::ofstream file(filename);
  if (file.is_open()) {
    Print(file);
    std::cout << "\n[ÉXITO] Horario guardado en: " << filename << std::endl;
  } else {
    std::cerr << "\n[ERROR] No se pudo crear el archivo: " << filename << std::endl;
  }
}

/**
 * @brief Guarda la salida JSON en un archivo
 */
void EmployeesSolutionPrint::SaveAsJSONToFile(const std::string& filename) const {
  std::ofstream file(filename);
  if (file.is_open()) {
    PrintAsJSON(file);
    std::cout << "\n[ÉXITO] Horario JSON guardado en: " << filename << std::endl;
  } else {
    std::cerr << "\n[ERROR] No se pudo crear el archivo JSON: " << filename << std::endl;
  }
}

void EmployeesSolutionPrint::PrintVisual(std::ostream& os) const {
  const auto& schedule = solution_.GetSchedule();
  int total_days = static_cast<int>(schedule.size());
  int total_shifts = static_cast<int>(schedule.empty() ? 0 : schedule[0].size());

  // Recoger qué turno trabaja cada empleado cada día (-1 si no trabaja)
  int total_employees = static_cast<int>(solution_.GetSchedule()[0].empty() ? 0
      : [&]() {
          int max_id = 0;
          for (const auto& day : schedule)
            for (const auto& shift : day)
              for (const auto& [emp_id, sat] : shift)
                max_id = std::max(max_id, emp_id);
          return max_id + 1;
        }());

  // Construir matriz assigned[emp][day] = turno trabajado o -1
  std::vector<std::vector<int>> assigned(
      total_employees, std::vector<int>(total_days, -1));

  for (int day = 0; day < total_days; ++day)
    for (int shift = 0; shift < total_shifts; ++shift)
      for (const auto& [emp_id, sat] : schedule[day][shift])
        assigned[emp_id][day] = shift;

  // Cabecera de días
  os << "         ";
  for (int day = 0; day < total_days; ++day) {
    os << "  Día " << std::setw(2) << std::left << (day + 1);
    for (int s = 1; s < total_shifts; ++s) os << "   ";
  }
  os << "\n";

  // Filas de empleados
  for (int emp = 0; emp < total_employees; ++emp) {
    os << "Emp " << std::setw(3) << std::left << (emp + 1) << "  ";
    for (int day = 0; day < total_days; ++day) {
      int worked_shift = assigned[emp][day];
      for (int shift = 0; shift < total_shifts; ++shift) {
        if (worked_shift == -1)
          os << "⬛ ";  // no trabaja ese día
        else if (worked_shift == shift)
          os << "🟩 ";  // turno trabajado
        else
          os << "⬜ ";  // turno libre ese día
      }
      os << " ";
    }
    os << "\n";
  }
  os << "\n";
}