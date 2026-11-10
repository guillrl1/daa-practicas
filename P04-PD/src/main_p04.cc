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
 * USO:
 *   ./bin/p04                          → experimentación completa
 *   ./bin/p04 instancia.json algo.json → una sola instancia con un algoritmo
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

#include "employees_instance.h"
#include "employees_solution.h"
#include "employees_solution_print.h"
#include "algorithm_factory.h"

namespace fs = std::filesystem;

static const std::string INSTANCES_DIR = "ejemplos/";
static const std::vector<std::pair<std::string,std::string>> ALGORITHMS_3 = {
  { "DyV+Voraz",     "algoritmos/divide_and_greedy.json"        },
  { "DyV+DP",        "algoritmos/divide_and_dynamic.json"       },
  { "DyV+DyV+Voraz", "algoritmos/divide_divide_and_greedy.json" },
};
static const std::vector<std::pair<std::string,std::string>> ALGORITHMS_5 = {
  { "DyV+Voraz",     "algoritmos/divide_and_greedy.json"        },
  { "DyV+DP",        "algoritmos/divide_and_dynamic.json"       },
  { "DyV+DyV+Voraz", "algoritmos/divide_divide_and_greedy.json" },
  { "DP-Multiday",   "algoritmos/dp_multiday.json"              },
  { "LocalSearch",   "algoritmos/local_search_dp.json"          },
};

struct AlgoResult {
  double objective = 0.0;
  double time_ms   = 0.0;
  bool   feasible  = false;
};

struct ExperimentRow {
  int                     employees;
  int                     days;
  int                     shifts;
  std::string             instance_name;
  std::vector<AlgoResult> results;
};

using AlgoList = std::vector<std::pair<std::string,std::string>>;

static std::string Basename(const std::string& path) {
  size_t slash = path.find_last_of("/\\");
  return (slash != std::string::npos) ? path.substr(slash + 1) : path;
}

static void PrintSep(int w) {
  std::cout << "  " << std::string(w, '-') << "\n";
}

static AlgoResult RunAlgorithm(const std::string& algo_json,
                                const EmployeesInstance& instance,
                                int horizon) {
  auto algorithm = AlgorithmFactory::FromFile(algo_json, instance.GetDaysOff(), horizon);

  auto t0 = std::chrono::high_resolution_clock::now();
  auto result = algorithm->Solve(&instance);
  auto t1 = std::chrono::high_resolution_clock::now();
  double ms = std::chrono::duration<double>(t1 - t0).count() * 1000.0;

  const auto* sol = dynamic_cast<const EmployeesSolution*>(result.get());
  if (!sol) throw std::runtime_error("Solución inválida");

  return { sol->ObjectiveFunction(), ms, sol->isFeasible() };
}

static void PrintTable(const std::vector<ExperimentRow>& rows, const AlgoList& ALGORITHMS) {
  if (rows.empty()) return;

  const int W_INST = 38;
  const int W_NUM  = 6;
  const int W_VAL  = 13;
  const int W_TIME = 12;
  int n_algos = static_cast<int>(ALGORITHMS.size());
  int TOTAL = W_INST + W_NUM * 3 + (W_VAL + W_TIME) * n_algos;

  std::cout << "\n  +" << std::string(TOTAL, '-') << "+\n";
  std::cout << "  |  TABLA DE EXPERIMENTACIÓN — Práctica 4"
            << std::string(TOTAL - 41, ' ') << "|\n";
  std::cout << "  +" << std::string(TOTAL, '-') << "+\n\n";

  std::cout << "  " << std::left
            << std::setw(W_INST) << "Instancia"
            << std::setw(W_NUM)  << "Emp"
            << std::setw(W_NUM)  << "Días"
            << std::setw(W_NUM)  << "Turn";
  for (const auto& [name, _] : ALGORITHMS)
    std::cout << std::setw(W_VAL) << ("f(" + name + ")") << std::setw(W_TIME) << "t(ms)";
  std::cout << "\n";
  PrintSep(TOTAL);

  for (const auto& row : rows) {
    std::cout << "  " << std::left
              << std::setw(W_INST) << row.instance_name.substr(0, W_INST - 1)
              << std::setw(W_NUM)  << row.employees
              << std::setw(W_NUM)  << row.days
              << std::setw(W_NUM)  << row.shifts;
    for (const auto& r : row.results)
      std::cout << std::setw(W_VAL)  << std::fixed << std::setprecision(0) << r.objective
                << std::setw(W_TIME) << std::setprecision(2) << r.time_ms;
    std::cout << "\n";
  }
  PrintSep(TOTAL);

  int n = static_cast<int>(rows.size());
  std::cout << "\n  PROMEDIOS (" << n << " instancias):\n";
  std::cout << "  " << std::left
            << std::setw(20) << "Algoritmo"
            << std::setw(16) << "f(x) promedio"
            << std::setw(16) << "T. prom (ms)" << "\n";
  PrintSep(52);

  for (int ai = 0; ai < n_algos; ++ai) {
    double sum_obj = 0, sum_ms = 0;
    for (const auto& row : rows) {
      sum_obj += row.results[ai].objective;
      sum_ms  += row.results[ai].time_ms;
    }
    std::cout << "  " << std::left
              << std::setw(20) << ALGORITHMS[ai].first
              << std::setw(16) << std::fixed << std::setprecision(1) << sum_obj / n
              << std::setw(16) << std::setprecision(3) << sum_ms / n << "\n";
  }
  PrintSep(52);

  std::vector<int> wins(n_algos, 0);
  for (const auto& row : rows) {
    double best = 0;
    for (const auto& r : row.results) best = std::max(best, r.objective);
    for (int ai = 0; ai < n_algos; ++ai)
      if (row.results[ai].objective >= best) wins[ai]++;
  }
  std::cout << "\n  Victorias:\n";
  for (int ai = 0; ai < n_algos; ++ai)
    std::cout << "    " << std::left << std::setw(20) << ALGORITHMS[ai].first
              << wins[ai] << "/" << n << "\n";
  std::cout << "\n";
}

static ExperimentRow RunExperiment(const std::string& instance_path, const AlgoList& ALGORITHMS) {
  EmployeesInstance instance(instance_path);

  int horizon = static_cast<int>(instance.GetRequiredEmployees().size());
  int shifts  = static_cast<int>(instance.GetRequiredEmployees()[0].size());
  int emp     = static_cast<int>(instance.GetDaysOff().size());

  ExperimentRow row;
  row.employees     = emp;
  row.days          = horizon;
  row.shifts        = shifts;
  row.instance_name = Basename(instance_path);

  for (const auto& [name, json_path] : ALGORITHMS) {
    try {
      row.results.push_back(RunAlgorithm(json_path, instance, horizon));
    } catch (const std::exception& ex) {
      std::cerr << "    [ERROR] " << name << ": " << ex.what() << "\n";
      row.results.push_back({0.0, 0.0, false});
    }
  }
  return row;
}

static void PrintVisualAllAlgorithms(const std::string& instance_path,
                                      const AlgoList& ALGORITHMS) {
  std::cout << "\n══════════════════════════════════════════\n"
            << "VISUALIZACIÓN — " << Basename(instance_path) << "\n"
            << "══════════════════════════════════════════\n";

  EmployeesInstance instance(instance_path);
  int horizon = static_cast<int>(instance.GetRequiredEmployees().size());

  for (const auto& [name, json_path] : ALGORITHMS) {
    try {
      auto algorithm = AlgorithmFactory::FromFile(json_path, instance.GetDaysOff(), horizon);
      auto result = algorithm->Solve(&instance);
      const auto* sol = dynamic_cast<const EmployeesSolution*>(result.get());
      if (!sol) continue;

      std::cout << "\n  --- " << name << " ---\n";
      EmployeesSolutionPrint(*sol).PrintVisual();
      std::cout << "  f(x)=" << std::fixed << std::setprecision(0) << sol->ObjectiveFunction()
                << "  factible=" << (sol->isFeasible() ? "SÍ" : "NO") << "\n";
    } catch (const std::exception& ex) {
      std::cerr << "  [ERROR] " << name << ": " << ex.what() << "\n";
    }
  }
}

int main(int argc, char* argv[]) {

  // Modo instancia única + algoritmo
  if (argc == 3) {
    std::string instance_file = argv[1];
    std::string algo_file     = argv[2];

    try {
      EmployeesInstance instance(instance_file);
      int horizon = static_cast<int>(instance.GetRequiredEmployees().size());

      auto algorithm = AlgorithmFactory::FromFile(algo_file, instance.GetDaysOff(), horizon);

      auto t0 = std::chrono::high_resolution_clock::now();
      auto result = algorithm->Solve(&instance);
      auto t1 = std::chrono::high_resolution_clock::now();
      double ms = std::chrono::duration<double>(t1 - t0).count() * 1000.0;

      const auto* sol = dynamic_cast<const EmployeesSolution*>(result.get());
      if (!sol) throw std::runtime_error("Solución inválida");

      EmployeesSolutionPrint(*sol).PrintVisual();
      std::cout << "\n  f(x)=" << sol->ObjectiveFunction()
                << "  t=" << ms << "ms"
                << "  factible=" << (sol->isFeasible() ? "SÍ" : "NO") << "\n\n";

    } catch (const std::exception& ex) {
      std::cerr << "Error: " << ex.what() << "\n";
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  // Modo experimentación completa
  std::string instances_dir = (argc == 2) ? argv[1] : INSTANCES_DIR;

  std::cout << "Selecciona modo de ejecución:\n"
            << "  1 - 3 algoritmos (DyV+Voraz, DyV+DP, DyV+DyV+Voraz)\n"
            << "  2 - 5 algoritmos (incluye DP-Multiday y LocalSearch)\n"
            << "Opción: ";
  int mode_choice = 1;
  std::cin >> mode_choice;

  const auto& ALGORITHMS = (mode_choice == 2) ? ALGORITHMS_5 : ALGORITHMS_3;
  std::cout << "Usando " << ALGORITHMS.size() << " algoritmos.\n\n";

  std::vector<std::string> instance_files;
  try {
    for (const auto& entry : fs::directory_iterator(instances_dir))
      if (entry.path().extension() == ".json")
        instance_files.push_back(entry.path().string());
  } catch (const std::exception& ex) {
    std::cerr << "Error leyendo directorio: " << ex.what() << "\n";
    return EXIT_FAILURE;
  }
  std::sort(instance_files.begin(), instance_files.end());

  std::cout << "Instancias: " << instance_files.size()
            << "  |  Algoritmos: " << ALGORITHMS.size() << "\n\n";

  std::vector<ExperimentRow> rows;
  for (const auto& path : instance_files) {
    std::cout << "[" << (rows.size() + 1) << "/" << instance_files.size()
              << "] " << Basename(path) << "\n";
    try {
      rows.push_back(RunExperiment(path, ALGORITHMS));
    } catch (const std::exception& ex) {
      std::cerr << "  Error: " << ex.what() << "\n";
    }
  }

  PrintTable(rows, ALGORITHMS);

  // Visualización de todos los algoritmos sobre la instancia más pequeña
  std::string smallest = instance_files.front();
  for (const auto& path : instance_files) {
    if (Basename(path).find("horizon7_employees5") != std::string::npos) {
      smallest = path;
      break;
    }
  }
  PrintVisualAllAlgorithms(smallest, ALGORITHMS);

  return EXIT_SUCCESS;
}