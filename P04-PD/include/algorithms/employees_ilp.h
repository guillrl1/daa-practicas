/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 08/03/2026
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
  int num_employees_ = 0;
  int num_days_      = 0;
  int num_shifts_    = 0;
  std::vector<unsigned int>                  days_off_;
  std::vector<std::vector<std::vector<int>>> satisfaction_;  // [emp][day][satisfaction]
  std::vector<std::vector<unsigned int>>     minimum_;       // [day][satisfaction]
  // Índice de x[emp][day][satisfaction] en el vector de columnas GLPK (base 1)
  int X(int emp, int day, int satisfaction) const {
    return 1 + emp * num_days_ * num_shifts_ + day * num_shifts_ + satisfaction;
  }
  // Índice de covered[day][satisfaction] (empieza tras todas las x)
  int C(int day, int satisfaction) const {
    return 1 + num_employees_ * num_days_ * num_shifts_ + day * num_shifts_ + satisfaction;
  }
  // Total de columnas
  int NumCols() const {
    return num_employees_ * num_days_ * num_shifts_ + num_days_ * num_shifts_;
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

  glp_prob* lp = glp_create_prob();
  glp_set_prob_name(lp, "employee_scheduling");
  glp_set_obj_dir(lp, GLP_MAX);

  glp_term_out(GLP_OFF);

  int total_cols = NumCols();
  glp_add_cols(lp, total_cols);

  for (int emp = 0; emp < num_employees_; ++emp) {
    for (int day = 0; day < num_days_; ++day) {
      for (int satisfaction = 0; satisfaction < num_shifts_; ++satisfaction) {
        int col = X(emp, day, satisfaction);
        std::string name = "x_" + std::to_string(emp) + "_"
                         + std::to_string(day) + "_" + std::to_string(satisfaction);
        glp_set_col_name(lp, col, name.c_str());
        glp_set_col_kind(lp, col, GLP_BV);  // binaria
        glp_set_obj_coef(lp, col, satisfaction_[emp][day][satisfaction]);
      }
    }
  }
  for (int day = 0; day < num_days_; ++day) {
    for (int satisfaction = 0; satisfaction < num_shifts_; ++satisfaction) {
      int col = C(day, satisfaction);
      std::string name = "cov_" + std::to_string(day) + "_" + std::to_string(satisfaction);
      glp_set_col_name(lp, col, name.c_str());
      glp_set_col_kind(lp, col, GLP_BV);
      glp_set_obj_coef(lp, col, 100.0);
    }
  }

  int num_rows = num_employees_ * num_days_
               + num_employees_
               + 2 * num_days_ * num_shifts_;
  glp_add_rows(lp, num_rows);

  int row = 1;

  for (int emp = 0; emp < num_employees_; ++emp) {
    for (int day = 0; day < num_days_; ++day) {
      std::string name = "one_shift_e" + std::to_string(emp) + "_d" + std::to_string(day);
      glp_set_row_name(lp, row, name.c_str());
      glp_set_row_bnds(lp, row, GLP_UP, 0.0, 1.0);  // <= 1

      std::vector<int>    idx(num_shifts_ + 1);
      std::vector<double> val(num_shifts_ + 1);
      for (int satisfaction = 0; satisfaction < num_shifts_; ++satisfaction) {
        idx[satisfaction+1] = X(emp, day, satisfaction);
        val[satisfaction+1] = 1.0;
      }
      glp_set_mat_row(lp, row, num_shifts_, idx.data(), val.data());
      ++row;
    }
  }

  for (int emp = 0; emp < num_employees_; ++emp) {
    std::string name = "rest_e" + std::to_string(emp);
    glp_set_row_name(lp, row, name.c_str());
    double max_work = static_cast<double>(num_days_ - static_cast<int>(days_off_[emp]));
    glp_set_row_bnds(lp, row, GLP_UP, 0.0, max_work);

    int n = num_days_ * num_shifts_;
    std::vector<int>    idx(n + 1);
    std::vector<double> val(n + 1);
    int k = 1;
    for (int day = 0; day < num_days_; ++day)
      for (int satisfaction = 0; satisfaction < num_shifts_; ++satisfaction) {
        idx[k] = X(emp, day, satisfaction);
        val[k] = 1.0;
        ++k;
      }
    glp_set_mat_row(lp, row, n, idx.data(), val.data());
    ++row;
  }

  for (int day = 0; day < num_days_; ++day) {
    for (int satisfaction = 0; satisfaction < num_shifts_; ++satisfaction) {
      int req = static_cast<int>(minimum_[day][satisfaction]);

      {
        std::string name = "cov_lb_d" + std::to_string(day) + "_t" + std::to_string(satisfaction);
        glp_set_row_name(lp, row, name.c_str());
        glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);  // >= 0

        std::vector<int>    idx(num_employees_ + 2);
        std::vector<double> val(num_employees_ + 2);
        for (int emp = 0; emp < num_employees_; ++emp) {
          idx[emp+1] = X(emp, day, satisfaction);
          val[emp+1] = 1.0;
        }
        idx[num_employees_+1] = C(day, satisfaction);
        val[num_employees_+1] = -static_cast<double>(req);
        glp_set_mat_row(lp, row, num_employees_ + 1, idx.data(), val.data());
        ++row;
      }

      {
        std::string name = "cov_ub_d" + std::to_string(day) + "_t" + std::to_string(satisfaction);
        glp_set_row_name(lp, row, name.c_str());
        glp_set_row_bnds(lp, row, GLP_UP, 0.0, 0.0);  // <= 0

        std::vector<int>    idx(num_employees_ + 2);
        std::vector<double> val(num_employees_ + 2);
        for (int emp = 0; emp < num_employees_; ++emp) {
          idx[emp + 1] = X(emp, day, satisfaction);
          val[emp + 1] = 1.0;
        }
        idx[num_employees_+1] = C(day, satisfaction);
        val[num_employees_+1] = -static_cast<double>(num_employees_);
        glp_set_mat_row(lp, row, num_employees_ + 1, idx.data(), val.data());
        ++row;
      }
    }
  }

  std::cout << "[ILP] Resolviendo con GLPK MIP solver...\n";

  glp_iocp params;
  glp_init_iocp(&params);
  params.presolve = GLP_ON;
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

  using ShiftAssignment = std::vector<std::pair<int,int>>;
  using DaySchedule     = std::vector<ShiftAssignment>;
  using FullSchedule    = std::vector<DaySchedule>;

  FullSchedule schedule(num_days_, DaySchedule(num_shifts_));

  for (int emp = 0; emp < num_employees_; ++emp) {
    for (int day = 0; day < num_days_; ++day) {
      for (int satisfaction = 0; satisfaction < num_shifts_; ++satisfaction) {
        double val = glp_mip_col_val(lp, X(emp, day, satisfaction));
        if (val > 0.5) {  // binaria: 1 si > 0.5
          schedule[day][satisfaction].push_back({emp, satisfaction_[emp][day][satisfaction]});
        }
      }
    }
  }
  glp_delete_prob(lp);
  return std::make_unique<EmployeesSolution>(schedule, days_off_, num_days_);
}

#endif // EMPLOYEES_ILP_H