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
 *
 * PATRÓN FACTORY
 * ──────────────
 * Lee un JSON de configuración de algoritmo y devuelve una instancia de
 * Algorithm correctamente configurada. Soporta composición recursiva,
 * lo que permite configuraciones anidadas como:
 *
 *   divide_and_greedy.json:
 *     { "class": "BinaryDivideAndConquer", "smallSolver": { "class": "Greedy" } }
 *
 *   divide_and_dynamic.json:
 *     { "class": "BinaryDivideAndConquer", "smallSolver": { "class": "DynamicProgramming" } }
 *
 *   divide_divide_and_greedy.json:
 *     { "class": "BinaryDivideAndConquer",
 *       "smallSolver": { "class": "BinaryDivideAndConquer",
 *                        "smallSolver": { "class": "Greedy" } } }
 *
 * PRINCIPIOS SOLID aplicados:
 *   - Open/Closed: añadir un nuevo algoritmo solo requiere añadir un
 *     bloque en el switch de Create(), sin modificar el resto.
 *   - Single Responsibility: AlgorithmFactory solo instancia, no ejecuta.
 *   - Dependency Inversion: el main depende de Algorithm (abstracción),
 *     no de las clases concretas.
 */

#ifndef ALGORITHM_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "algorithm.h"
#include "employees_greedy_solver.h"
#include "employees_dp_solver.h"
#include "employees_dp_multiday.h"
#include "employees_local_search.h"
#include "employees_divide_and_conquer.h"

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <iostream>

/**
 * @class AlgorithmFactory
 * @brief Fábrica de algoritmos a partir de JSON. Permite crear cualquier
 *        algoritmo definido en el proyecto, con la configuración deseada.
 */
class AlgorithmFactory {
 public:
  /**
   * @brief Crea un algoritmo a partir de un fichero JSON de configuración.
   * @param json_filename Ruta al JSON ("divide_and_greedy.json" por ejemplo).
   * @param days_off Días de descanso globales de los empleados.
   * @param horizon Horizonte total de planificación.
   * @return unique_ptr al algoritmo configurado.
   */
  static std::unique_ptr<Algorithm> FromFile(
      const std::string& json_filename,
      const std::vector<unsigned int>& days_off,
      int horizon) {

    std::ifstream input_file(json_filename);
    if (!input_file.is_open()) {
      throw std::runtime_error("AlgorithmFactory: no se puede abrir " + json_filename);
    }
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    return FromJson(buffer.str(), days_off, horizon);
  }

  /**
   * @brief Crea un algoritmo a partir de una cadena JSON (permite recursión).
   * @param json Contenido JSON como string.
   * @param days_off Días de descanso globales.
   * @param horizon Horizonte total.
   * @return unique_ptr al algoritmo configurado.
   */
  static std::unique_ptr<Algorithm> FromJson(
      const std::string& json,
      const std::vector<unsigned int>& days_off,
      int horizon) {

    // Extraemos "class": "NombreClase"
    std::string class_name = ExtractString(json, "class");
    // std::cout << "[Factory] Creando algoritmo: " << class_name << "\n";

    if (class_name == "Greedy") {
      return std::make_unique<EmployeesGreedySolver>(days_off, horizon);
    }

    if (class_name == "DynamicProgramming") {
      return std::make_unique<EmployeesDPSolver>(days_off, horizon);
    }

    if (class_name == "BinaryDivideAndConquer") {
      std::string small_json = ExtractObject(json, "smallSolver");
      if (small_json.empty()) {
        throw std::runtime_error(
          "AlgorithmFactory: BinaryDivideAndConquer requiere 'smallSolver'"
        );
      }
      std::string combine_json = ExtractObject(json, "combineSolver");
      if (combine_json.empty()) {
        throw std::runtime_error(
          "AlgorithmFactory: BinaryDivideAndConquer requiere 'combineSolver'"
        );
      }
      // El smallSolver puede ser cualquier algoritmo
      auto small_solver = FromJson(small_json, days_off, horizon);
      auto combine_solver = FromJson(combine_json, days_off, horizon); // Reutilizamos el mismo JSON para el CombineSolver

      // Combine solver
      return std::make_unique<EmployeesDivideAndConquer>(
        std::move(small_solver), std::move(combine_solver), days_off, horizon
      );
    }

    if (class_name == "DynamicProgrammingMultiday") {
      return std::make_unique<EmployeesDPMultiday>();
    }

    if (class_name == "LocalSearch") {
      std::string base_json = ExtractObject(json, "baseSolver");
      if (base_json.empty()) {
        throw std::runtime_error(
          "AlgorithmFactory: LocalSearch requiere 'baseSolver'"
        );
      }
      auto base_solver = FromJson(base_json, days_off, horizon);
      return std::make_unique<EmployeesLocalSearch>(std::move(base_solver));
    }
    throw std::runtime_error(
      "AlgorithmFactory: clase desconocida '" + class_name + "'"
    );
  }

 private:
  /**
   * @brief Extrae el valor de una clave string del JSON.
   *        Busca el patrón: "key": "value"
   */
  static std::string ExtractString(const std::string& json, const std::string& key) {
    std::regex json_regex("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");
    std::smatch match;
    if (std::regex_search(json, match, json_regex)) {
      return match[1].str();
    }
    return "";
  }

  /**
   * @brief Extrae el bloque objeto JSON asociado a una clave.
   *        Busca "key": { ... } respetando llaves anidadas.
   */
  static std::string ExtractObject(const std::string& json, const std::string& key) {
    std::string pattern = "\"" + key + "\"\\s*:\\s*\\{";
    std::regex  regex_pattern(pattern);
    std::smatch match;
    // Si no se encuentra la clave o no es un objeto, se devuelve string vacío
    if (!std::regex_search(json, match, regex_pattern)) return "";
    // Posición de la llave de apertura '{' del objeto
    size_t start = match.position() + match.length() - 1;
    int depth = 0;
    size_t index  = start;
    while (index < json.size()) {
      if (json[index] == '{') {
        ++depth;
      } else if (json[index] == '}') {
        --depth;
        if (depth == 0) return json.substr(start, index - start + 1);
      }
      ++index;
    }
    return "";
  }
};

#endif // ALGORITHM_FACTORY_H