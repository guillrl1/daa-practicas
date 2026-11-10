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
 * BENCHMARK ÓPTIMO: Programación Lineal Entera con GLPK
 * ──────────────────────────────────────────────────────
 * Resuelve el problema de planificación de empleados de forma EXACTA
 * formulándolo como un ILP (Integer Linear Program) y resolviéndolo
 * con el solver GLPK (GNU Linear Programming Kit).
 *
 * REQUISITO: instalar GLPK
 *   macOS:   brew install glpk
 *   Ubuntu:  sudo apt install libglpk-dev
 *   Compilar añadiendo: -lglpk
 *
 * FORMULACIÓN ILP
 * ───────────────
 * Variables de decisión:
 *   x[e][d][t] ∈ {0,1}  →  1 si el empleado e trabaja el turno t del día d
 *
 * Función objetivo (maximizar):
 *   MAX  SUM(e,d,t) { sat[e][d][t] * x[e][d][t] }
 *      + SUM(d,t)   { 100 * covered[d][t] }
 *
 *   donde covered[d][t] es una variable auxiliar binaria que vale 1 si
 *   el turno (d,t) tiene al menos minimum[d][t] empleados asignados.
 *
 * Restricciones:
 *   R1 - Un turno por día por empleado:
 *        SUM(t) x[e][d][t] <= 1   para todo e, d
 *
 *   R2 - Días de descanso obligatorios (restricción DURA):
 *        SUM(d,t) x[e][d][t] <= horizon - days_off[e]   para todo e
 *
 *   R3 - Cobertura mínima de turnos (vincula x con covered):
 *        SUM(e) x[e][d][t] >= minimum[d][t] * covered[d][t]   para todo d,t
 *        SUM(e) x[e][d][t] <= E * covered[d][t]               para todo d,t
 *        (covered[d][t]=1 ↔ turno cubierto con el mínimo)
 *
 * GLPK resuelve este ILP con su propio Branch & Bound interno altamente
 * optimizado, garantizando la solución óptima global.
 */

#ifndef EMPLOYEES_ILP_H
#define EMPLOYEES_ILP_H

#include "algorithm.h"
#include "employees_instance.h"
#include "employees_solution.h"

#include <glpk.h>

#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>

class EmployeesILP : public Algorithm {
 public:
  EmployeesILP() = default;
  std::unique_ptr<Solution> Solve(const Instance* instance) override;

 private:
  // ── Datos de la instancia ──────────────────────────────────────────────────
  int num_employees_ = 0;
  int num_days_      = 0;
  int num_shifts_    = 0;
  std::vector<unsigned int>                  days_off_;
  std::vector<std::vector<std::vector<int>>> satisfaction_;  // [e][d][t]
  std::vector<std::vector<unsigned int>>     minimum_;       // [d][t]

  // ── Helpers de índices ─────────────────────────────────────────────────────
  // Índice de x[e][d][t] en el vector de columnas GLPK (base 1)
  int X(int e, int d, int t) const {
    return 1 + e * num_days_ * num_shifts_ + d * num_shifts_ + t;
  }
  // Índice de covered[d][t] (empieza tras todas las x)
  int C(int d, int t) const {
    return 1 + num_employees_ * num_days_ * num_shifts_ + d * num_shifts_ + t;
  }

  // Total de columnas
  int NumCols() const {
    return num_employees_ * num_days_ * num_shifts_   // variables x
         + num_days_ * num_shifts_;                    // variables covered
  }
};

// ─────────────────────────────────────────────────────────────────────────────
inline std::unique_ptr<Solution> EmployeesILP::Solve(const Instance* instance) {
  const auto* emp = dynamic_cast<const EmployeesInstance*>(instance);
  if (!emp) return nullptr;

  satisfaction_  = emp->GetSatisfactionRates();
  minimum_       = emp->GetRequiredEmployees();
  days_off_      = emp->GetDaysOff();
  num_employees_ = static_cast<int>(days_off_.size());
  num_days_      = static_cast<int>(minimum_.size());
  num_shifts_    = static_cast<int>(minimum_[0].size());

  std::cout << "[ILP] Empleados=" << num_employees_
            << "  Días="          << num_days_
            << "  Turnos="        << num_shifts_ << "\n"
            << "[ILP] Construyendo modelo GLPK...\n";

  // ── Crear problema GLPK ────────────────────────────────────────────────────
  glp_prob* lp = glp_create_prob();
  glp_set_prob_name(lp, "employee_scheduling");
  glp_set_obj_dir(lp, GLP_MAX);

  // Silenciar output de GLPK (lo controlamos nosotros)
  glp_term_out(GLP_OFF);

  // ── Columnas (variables de decisión) ──────────────────────────────────────
  int total_cols = NumCols();
  glp_add_cols(lp, total_cols);

  // x[e][d][t]: binaria, coeficiente = satisfacción
  for (int e = 0; e < num_employees_; ++e) {
    for (int d = 0; d < num_days_; ++d) {
      for (int t = 0; t < num_shifts_; ++t) {
        int col = X(e, d, t);
        std::string name = "x_" + std::to_string(e) + "_"
                         + std::to_string(d) + "_" + std::to_string(t);
        glp_set_col_name(lp, col, name.c_str());
        glp_set_col_kind(lp, col, GLP_BV);  // binaria
        glp_set_obj_coef(lp, col, satisfaction_[e][d][t]);
      }
    }
  }

  // covered[d][t]: binaria, coeficiente = 100
  for (int d = 0; d < num_days_; ++d) {
    for (int t = 0; t < num_shifts_; ++t) {
      int col = C(d, t);
      std::string name = "cov_" + std::to_string(d) + "_" + std::to_string(t);
      glp_set_col_name(lp, col, name.c_str());
      glp_set_col_kind(lp, col, GLP_BV);
      glp_set_obj_coef(lp, col, 100.0);
    }
  }

  // ── Filas (restricciones) ──────────────────────────────────────────────────
  // Número total de restricciones:
  //   R1: num_employees * num_days
  //   R2: num_employees
  //   R3a + R3b: 2 * num_days * num_shifts
  int num_rows = num_employees_ * num_days_
               + num_employees_
               + 2 * num_days_ * num_shifts_;
  glp_add_rows(lp, num_rows);

  int row = 1;

  // R1: SUM(t) x[e][d][t] <= 1  (un turno por día por empleado)
  for (int e = 0; e < num_employees_; ++e) {
    for (int d = 0; d < num_days_; ++d) {
      std::string name = "one_shift_e" + std::to_string(e) + "_d" + std::to_string(d);
      glp_set_row_name(lp, row, name.c_str());
      glp_set_row_bnds(lp, row, GLP_UP, 0.0, 1.0);  // <= 1

      std::vector<int>    idx(num_shifts_ + 1);
      std::vector<double> val(num_shifts_ + 1);
      for (int t = 0; t < num_shifts_; ++t) {
        idx[t+1] = X(e, d, t);
        val[t+1] = 1.0;
      }
      glp_set_mat_row(lp, row, num_shifts_, idx.data(), val.data());
      ++row;
    }
  }

  // R2: SUM(d,t) x[e][d][t] <= horizon - days_off[e]  (descansos obligatorios)
  for (int e = 0; e < num_employees_; ++e) {
    std::string name = "rest_e" + std::to_string(e);
    glp_set_row_name(lp, row, name.c_str());
    double max_work = static_cast<double>(num_days_ - static_cast<int>(days_off_[e]));
    glp_set_row_bnds(lp, row, GLP_UP, 0.0, max_work);

    int n = num_days_ * num_shifts_;
    std::vector<int>    idx(n + 1);
    std::vector<double> val(n + 1);
    int k = 1;
    for (int d = 0; d < num_days_; ++d)
      for (int t = 0; t < num_shifts_; ++t) {
        idx[k] = X(e, d, t);
        val[k] = 1.0;
        ++k;
      }
    glp_set_mat_row(lp, row, n, idx.data(), val.data());
    ++row;
  }

  // R3: vinculación entre x y covered
  for (int d = 0; d < num_days_; ++d) {
    for (int t = 0; t < num_shifts_; ++t) {
      int req = static_cast<int>(minimum_[d][t]);

      // R3a: SUM(e) x[e][d][t] >= req * covered[d][t]
      //   ↔  SUM(e) x[e][d][t] - req * covered[d][t] >= 0
      {
        std::string name = "cov_lb_d" + std::to_string(d) + "_t" + std::to_string(t);
        glp_set_row_name(lp, row, name.c_str());
        glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);  // >= 0

        std::vector<int>    idx(num_employees_ + 2);
        std::vector<double> val(num_employees_ + 2);
        for (int e = 0; e < num_employees_; ++e) {
          idx[e+1] = X(e, d, t);
          val[e+1] = 1.0;
        }
        idx[num_employees_+1] = C(d, t);
        val[num_employees_+1] = -static_cast<double>(req);
        glp_set_mat_row(lp, row, num_employees_ + 1, idx.data(), val.data());
        ++row;
      }

      // R3b: SUM(e) x[e][d][t] <= E * covered[d][t]
      //   ↔  SUM(e) x[e][d][t] - E * covered[d][t] <= 0
      {
        std::string name = "cov_ub_d" + std::to_string(d) + "_t" + std::to_string(t);
        glp_set_row_name(lp, row, name.c_str());
        glp_set_row_bnds(lp, row, GLP_UP, 0.0, 0.0);  // <= 0

        std::vector<int>    idx(num_employees_ + 2);
        std::vector<double> val(num_employees_ + 2);
        for (int e = 0; e < num_employees_; ++e) {
          idx[e+1] = X(e, d, t);
          val[e+1] = 1.0;
        }
        idx[num_employees_+1] = C(d, t);
        val[num_employees_+1] = -static_cast<double>(num_employees_);
        glp_set_mat_row(lp, row, num_employees_ + 1, idx.data(), val.data());
        ++row;
      }
    }
  }

  // ── Resolver con MIP (Branch & Bound interno de GLPK) ─────────────────────
  std::cout << "[ILP] Resolviendo con GLPK MIP solver...\n";

  glp_iocp params;
  glp_init_iocp(&params);
  params.presolve = GLP_ON;   // preprocesado automático
  params.msg_lev  = GLP_MSG_OFF;

  int status = glp_intopt(lp, &params);

  if (status != 0 && status != GLP_EMIPGAP) {
    glp_delete_prob(lp);
    throw std::runtime_error("[ILP] GLPK no pudo resolver el problema. Código: "
                             + std::to_string(status));
  }

  int mip_status = glp_mip_status(lp);
  if (mip_status != GLP_OPT && mip_status != GLP_FEAS) {
    glp_delete_prob(lp);
    throw std::runtime_error("[ILP] No se encontró solución feasible.");
  }

  double obj_val = glp_mip_obj_val(lp);
  std::cout << "[ILP] Solución óptima encontrada. f(x) = " << obj_val << "\n";

  // ── Reconstruir EmployeesSolution desde las variables x ───────────────────
  using ShiftAssignment = std::vector<std::pair<int,int>>;
  using DaySchedule     = std::vector<ShiftAssignment>;
  using FullSchedule    = std::vector<DaySchedule>;

  FullSchedule schedule(num_days_, DaySchedule(num_shifts_));

  for (int e = 0; e < num_employees_; ++e) {
    for (int d = 0; d < num_days_; ++d) {
      for (int t = 0; t < num_shifts_; ++t) {
        double val = glp_mip_col_val(lp, X(e, d, t));
        if (val > 0.5) {  // binaria: 1 si > 0.5
          schedule[d][t].push_back({e, satisfaction_[e][d][t]});
        }
      }
    }
  }

  glp_delete_prob(lp);

  return std::make_unique<EmployeesSolution>(schedule, days_off_, num_days_);
}

#endif // EMPLOYEES_ILP_H