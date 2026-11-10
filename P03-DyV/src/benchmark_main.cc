/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 08/03/2026
 *
 * BENCHMARK: ILP óptimo (GLPK) vs Divide y Vencerás (heurístico)
 *
 * USO:
 *   ./benchmark <ruta_json>
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>

#include "employees_instance.h"
#include "employees_algorithm.h"
#include "employees_solution.h"
#include "employees_solution_print.h"
#include "employees_ilp.h"

// ─── Utilidades ───────────────────────────────────────────────────────────────
static void PrintSection(const std::string& title) {
  std::cout << "\n╔══════════════════════════════════════════════════════════╗\n"
            << "║  " << std::left << std::setw(57) << title << "║\n"
            << "╚══════════════════════════════════════════════════════════╝\n";
}

static void PrintSummary(const std::string& name,
                          const EmployeesSolution* sol,
                          const EmployeesInstance& inst,
                          double ms) {
  int n       = static_cast<int>(inst.GetDaysOff().size());
  int horizon = static_cast<int>(sol->GetSchedule().size());

  std::vector<int> days_worked(n, 0);
  for (const auto& day : sol->GetSchedule()) {
    std::vector<bool> worked(n, false);
    for (const auto& shift : day)
      for (const auto& [e, s] : shift)
        worked[e] = true;
    for (int e = 0; e < n; ++e)
      if (worked[e]) days_worked[e]++;
  }

  std::cout << "\n  Algoritmo  : " << name
            << "\n  Tiempo     : " << std::fixed << std::setprecision(4) << ms << " ms"
            << "\n  f(x)       : " << sol->ObjectiveFunction()
            << "\n  Factible   : " << (sol->isFeasible() ? "SÍ ✓" : "NO ✗")
            << "\n  Días trabajados por empleado:\n";
  for (int e = 0; e < n; ++e) {
    int max_allowed = horizon - static_cast<int>(inst.GetDaysOff()[e]);
    std::cout << "    Empleado " << e << ": "
              << days_worked[e] << " / " << max_allowed << " días\n";
  }
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
  std::string json_path;
  if (argc >= 2) {
    json_path = argv[1];
  } else {
    std::cout << "Ruta del fichero JSON: ";
    std::cin >> json_path;
  }

  // ── Cargar instancia ───────────────────────────────────────────────────────
  PrintSection("CARGA DE INSTANCIA");
  EmployeesInstance instance;
  try {
    instance = EmployeesInstance(json_path);
  } catch (const std::exception& ex) {
    std::cerr << "Error al cargar instancia: " << ex.what() << "\n";
    return EXIT_FAILURE;
  }

  int horizon    = static_cast<int>(instance.GetRequiredEmployees().size());
  int num_emp    = static_cast<int>(instance.GetDaysOff().size());
  int num_shifts = static_cast<int>(instance.GetRequiredEmployees()[0].size());

  std::cout << "\n  Horizonte (días) : " << horizon    << "\n"
            <<   "  Empleados        : " << num_emp    << "\n"
            <<   "  Turnos/día       : " << num_shifts << "\n";

  // ── ILP óptimo ────────────────────────────────────────────────────────────
  PrintSection("ALGORITMO: ILP con GLPK (ÓPTIMO GARANTIZADO)");
  EmployeesILP ilp_solver;
  std::unique_ptr<Solution> ilp_result;

  auto t0 = std::chrono::high_resolution_clock::now();
  try {
    ilp_result = ilp_solver.Solve(&instance);
  } catch (const std::exception& ex) {
    std::cerr << "Error en ILP: " << ex.what() << "\n";
    return EXIT_FAILURE;
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  double ilp_ms = std::chrono::duration<double>(t1 - t0).count() * 1000.0;

  const auto* ilp_sol = dynamic_cast<const EmployeesSolution*>(ilp_result.get());
  if (!ilp_sol) {
    std::cerr << "Error: ILP no devolvió solución válida.\n";
    return EXIT_FAILURE;
  }
  PrintSummary("ILP (GLPK)", ilp_sol, instance, ilp_ms);
  std::cout << "\n  --- Horario óptimo ---\n";
  EmployeesSolutionPrint(*ilp_sol).Print();

  // ── Divide y Vencerás ─────────────────────────────────────────────────────
  PrintSection("ALGORITMO: DIVIDE Y VENCERÁS (HEURÍSTICO)");
  auto dyv = std::make_unique<EmployeesAlgorithm>(instance.GetDaysOff(), horizon);

  auto t2 = std::chrono::high_resolution_clock::now();
  auto dyv_result = dyv->Solve(&instance);
  auto t3 = std::chrono::high_resolution_clock::now();
  double dyv_ms = std::chrono::duration<double>(t3 - t2).count() * 1000.0;

  const auto* dyv_sol = dynamic_cast<const EmployeesSolution*>(dyv_result.get());
  if (!dyv_sol) {
    std::cerr << "Error: DyV no devolvió solución válida.\n";
    return EXIT_FAILURE;
  }
  PrintSummary("Divide y Vencerás", dyv_sol, instance, dyv_ms);
  std::cout << "\n  --- Horario DyV ---\n";
  EmployeesSolutionPrint(*dyv_sol).Print();

  // ── Comparativa ───────────────────────────────────────────────────────────
  PrintSection("COMPARATIVA ILP (ÓPTIMO) vs DIVIDE Y VENCERÁS");

  double ilp_obj = ilp_sol->ObjectiveFunction();
  double dyv_obj = dyv_sol->ObjectiveFunction();
  double gap_pct = (ilp_obj != 0.0)
                 ? ((ilp_obj - dyv_obj) / std::abs(ilp_obj)) * 100.0
                 : 0.0;

  std::cout << "\n  ┌──────────────────────────────────────────────────┐\n"
            << "  │  Métrica               ILP (GLPK)      DyV       │\n"
            << "  ├──────────────────────────────────────────────────┤\n"
            << "  │  f(x)           "
            << std::setw(16) << std::fixed << std::setprecision(1) << ilp_obj
            << std::setw(11) << dyv_obj << "  │\n"
            << "  │  Tiempo (ms)    "
            << std::setw(16) << std::setprecision(4) << ilp_ms
            << std::setw(11) << dyv_ms << "  │\n"
            << "  │  Factible       "
            << std::setw(16) << (ilp_sol->isFeasible() ? "SÍ" : "NO")
            << std::setw(11) << (dyv_sol->isFeasible() ? "SÍ" : "NO") << "  │\n"
            << "  └──────────────────────────────────────────────────┘\n";

  std::cout << "\n  ► GAP de optimalidad (DyV vs Óptimo): "
            << std::setprecision(2) << gap_pct << "%\n";

  if      (gap_pct <= 0.0)  std::cout << "  ✅ DyV encontró la solución ÓPTIMA.\n";
  else if (gap_pct <= 5.0)  std::cout << "  ✅ DyV está muy cerca del óptimo (≤5%).\n";
  else if (gap_pct <= 15.0) std::cout << "  ⚠️  DyV tiene desviación moderada del óptimo.\n";
  else                       std::cout << "  ❌ DyV se aleja significativamente del óptimo.\n";

  std::cout << "\n  Speedup DyV vs ILP: x"
            << std::setprecision(1) << (ilp_ms / (dyv_ms + 1e-9)) << "\n\n";

  return EXIT_SUCCESS;
}