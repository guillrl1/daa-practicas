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
 *   ./bin/benchmark ejemplos/
 *   ./bin/benchmark ejemplos/instancia.json
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
#include "employees_ilp.h"
#include "algorithm_factory.h"

namespace fs = std::filesystem;

static const std::vector<std::pair<std::string,std::string>> ALGORITHMS = {
  { "DyV+Voraz",     "algoritmos/divide_and_greedy.json"        },
  { "DyV+DP",        "algoritmos/divide_and_dynamic.json"       },
  { "DyV+DyV+Voraz", "algoritmos/divide_divide_and_greedy.json" },
  { "DP-Multiday",   "algoritmos/dp_multiday.json"              },
  { "LocalSearch",   "algoritmos/local_search_dp.json"          },
};

struct AlgoResult {
  double objective = 0.0;
  double time_ms = 0.0;
  double gap_pct = 0.0;
};

struct BenchmarkRow {
  std::string instance_name;
  int employees;
  int days;
  int shifts;
  double ilp_objective = 0.0;
  double ilp_time_ms = 0.0;
  std::vector<AlgoResult> results;
};

static std::string Basename(const std::string& path) {
  size_t slash = path.find_last_of("/\\");
  return (slash != std::string::npos) ? path.substr(slash + 1) : path;
}

static AlgoResult RunAlgorithm(const std::string& algo_json,
                                const EmployeesInstance& instance,
                                int horizon,
                                double ilp_objective) {
  auto algorithm = AlgorithmFactory::FromFile(algo_json, instance.GetDaysOff(), horizon);

  auto t0 = std::chrono::high_resolution_clock::now();
  auto result = algorithm->Solve(&instance);
  auto t1 = std::chrono::high_resolution_clock::now();
  double ms = std::chrono::duration<double>(t1 - t0).count() * 1000.0;

  const auto* sol = dynamic_cast<const EmployeesSolution*>(result.get());
  if (!sol) throw std::runtime_error("Solución inválida");

  double objective = sol->ObjectiveFunction();
  double gap = (ilp_objective > 0.0)
      ? (ilp_objective - objective) / ilp_objective * 100.0
      : 0.0;

  return { objective, ms, gap };
}

static BenchmarkRow RunBenchmark(const std::string& instance_path) {
  EmployeesInstance instance(instance_path);

  int horizon = static_cast<int>(instance.GetRequiredEmployees().size());
  int shifts  = static_cast<int>(instance.GetRequiredEmployees()[0].size());
  int emp     = static_cast<int>(instance.GetDaysOff().size());

  BenchmarkRow row;
  row.instance_name = Basename(instance_path);
  row.employees = emp;
  row.days = horizon;
  row.shifts = shifts;

  // Primero los 5 algoritmos (sin gap aún, ILP no disponible todavía)
  for (const auto& [name, json_path] : ALGORITHMS) {
    try {
      row.results.push_back(RunAlgorithm(json_path, instance, horizon, 0.0));
    } catch (const std::exception& ex) {
      std::cerr << "    [ERROR] " << name << ": " << ex.what() << "\n";
      row.results.push_back({0.0, 0.0, 0.0});
    }
  }

  // ILP al final como referencia óptima
  std::cout << "  [ILP] resolviendo " << row.instance_name << "...\n";
  EmployeesILP ilp;
  auto t0 = std::chrono::high_resolution_clock::now();
  auto ilp_result = ilp.Solve(&instance);
  auto t1 = std::chrono::high_resolution_clock::now();
  row.ilp_time_ms = std::chrono::duration<double>(t1 - t0).count() * 1000.0;

  const auto* ilp_sol = dynamic_cast<const EmployeesSolution*>(ilp_result.get());
  if (!ilp_sol) throw std::runtime_error("ILP no devolvió solución válida");
  row.ilp_objective = ilp_sol->ObjectiveFunction();

  // Recalcular gaps con el óptimo ya conocido
  for (auto& r : row.results) {
    r.gap_pct = (row.ilp_objective > 0.0)
        ? (row.ilp_objective - r.objective) / row.ilp_objective * 100.0
        : 0.0;
  }

  return row;
}

static void PrintTable(const std::vector<BenchmarkRow>& rows) {
  if (rows.empty()) return;

  const int W_INST = 36;
  const int W_NUM  = 5;
  const int W_FX   = 10;
  const int W_T    = 10;
  const int W_GAP  = 8;
  const int W_ILP  = 10;

  int n_algos = static_cast<int>(ALGORITHMS.size());
  int total_width = W_INST + W_NUM * 3 + (W_FX + W_T + W_GAP) * n_algos + W_ILP + W_T;

  auto sep = [&]() { std::cout << "  " << std::string(total_width, '-') << "\n"; };

  std::cout << "\n";
  sep();
  std::cout << "  BENCHMARK: 5 algoritmos vs ILP (optimo exacto)\n";
  sep();

  // Cabecera fila 1: nombres algoritmos + ILP al final
  std::cout << "  " << std::left << std::setw(W_INST + W_NUM * 3) << "";
  for (const auto& [name, _] : ALGORITHMS)
    std::cout << std::setw(W_FX + W_T + W_GAP) << name;
  std::cout << std::setw(W_ILP + W_T) << "ILP (opt)" << "\n";

  // Cabecera fila 2: columnas
  std::cout << "  " << std::left
            << std::setw(W_INST) << "Instancia"
            << std::setw(W_NUM)  << "Emp"
            << std::setw(W_NUM)  << "Dias"
            << std::setw(W_NUM)  << "Turn";
  for (int i = 0; i < n_algos; ++i)
    std::cout << std::setw(W_FX)  << "f(x)"
              << std::setw(W_T)   << "t(ms)"
              << std::setw(W_GAP) << "gap%";
  std::cout << std::setw(W_ILP) << "f(x)"
            << std::setw(W_T)   << "t(ms)" << "\n";
  sep();

  // Filas de datos
  for (const auto& row : rows) {
    std::cout << "  " << std::left
              << std::setw(W_INST) << row.instance_name.substr(0, W_INST - 1)
              << std::setw(W_NUM)  << row.employees
              << std::setw(W_NUM)  << row.days
              << std::setw(W_NUM)  << row.shifts;
    for (const auto& r : row.results)
      std::cout << std::setw(W_FX)  << std::fixed << std::setprecision(0) << r.objective
                << std::setw(W_T)   << std::setprecision(1) << r.time_ms
                << std::setw(W_GAP) << std::setprecision(1) << r.gap_pct;
    std::cout << std::setw(W_ILP) << std::setprecision(0) << row.ilp_objective
              << std::setw(W_T)   << std::setprecision(1) << row.ilp_time_ms << "\n";
  }
  sep();

  // Promedios
  int n = static_cast<int>(rows.size());
  std::cout << "\n  PROMEDIOS (" << n << " instancias):\n";
  std::cout << "  " << std::left
            << std::setw(20) << "Algoritmo"
            << std::setw(14) << "f(x) prom"
            << std::setw(14) << "t prom (ms)"
            << std::setw(10) << "gap% prom" << "\n";
  sep();

  for (int ai = 0; ai < n_algos; ++ai) {
    double sum_obj = 0, sum_ms = 0, sum_gap = 0;
    for (const auto& row : rows) {
      sum_obj += row.results[ai].objective;
      sum_ms  += row.results[ai].time_ms;
      sum_gap += row.results[ai].gap_pct;
    }
    std::cout << "  " << std::left
              << std::setw(20) << ALGORITHMS[ai].first
              << std::setw(14) << std::fixed << std::setprecision(1) << sum_obj / n
              << std::setw(14) << std::setprecision(2) << sum_ms / n
              << std::setw(10) << std::setprecision(2) << sum_gap / n << "\n";
  }

  double sum_ilp_obj = 0, sum_ilp_ms = 0;
  for (const auto& row : rows) {
    sum_ilp_obj += row.ilp_objective;
    sum_ilp_ms  += row.ilp_time_ms;
  }
  std::cout << "  " << std::left
            << std::setw(20) << "ILP (optimo)"
            << std::setw(14) << std::fixed << std::setprecision(1) << sum_ilp_obj / n
            << std::setw(14) << std::setprecision(2) << sum_ilp_ms / n
            << std::setw(10) << "0.00" << "\n";
  sep();
  std::cout << "\n";
}

int main(int argc, char* argv[]) {
  std::string input = (argc >= 2) ? argv[1] : "ejemplos/";

  std::vector<std::string> instance_files;

  if (fs::is_regular_file(input)) {
    instance_files.push_back(input);
  } else {
    try {
      for (const auto& entry : fs::directory_iterator(input))
        if (entry.path().extension() == ".json")
          instance_files.push_back(entry.path().string());
    } catch (const std::exception& ex) {
      std::cerr << "Error leyendo directorio: " << ex.what() << "\n";
      return EXIT_FAILURE;
    }
    std::sort(instance_files.begin(), instance_files.end());
  }

  std::cout << "Benchmark: " << instance_files.size()
            << " instancias  |  " << ALGORITHMS.size() << " algoritmos + ILP\n\n";

  std::vector<BenchmarkRow> rows;
  for (size_t i = 0; i < instance_files.size(); ++i) {
    std::cout << "[" << (i + 1) << "/" << instance_files.size()
              << "] " << Basename(instance_files[i]) << "\n";
    try {
      rows.push_back(RunBenchmark(instance_files[i]));
    } catch (const std::exception& ex) {
      std::cerr << "  Error: " << ex.what() << "\n";
    }
  }

  PrintTable(rows);
  return EXIT_SUCCESS;
}