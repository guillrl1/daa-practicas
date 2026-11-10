/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 * Guillermo López Concepción alu0101620459@ull.edu.es
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