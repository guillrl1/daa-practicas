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

#ifndef EMPLOYEES_SOLUTION_PRINT_H
#define EMPLOYEES_SOLUTION_PRINT_H

#include "employees_solution.h"
#include <iostream>
#include <string>

class EmployeesSolutionPrint {
 public:
  EmployeesSolutionPrint(const EmployeesSolution& solution) : solution_(solution) {}
  
  void Print(std::ostream& os = std::cout) const;
  void PrintAsJSON(std::ostream& os = std::cout) const;
  
  void SaveToFile(const std::string& filename) const;
  void SaveAsJSONToFile(const std::string& filename) const;

  void PrintVisual(std::ostream& os = std::cout) const;

 private:
  EmployeesSolution solution_;
};

#endif // EMPLOYEES_SOLUTION_PRINT_H