/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 1/03/2026
 */

 #ifndef EMPLOYEES_ALGORITHM_H
 #define EMPLOYEES_ALGORITHM_H
 
 #include "divide_and_conquer.h"
 #include "employees_instance.h"
 #include "employees_solution.h"
 
 #include <vector>
 #include <memory>
 
class EmployeesAlgorithm : public DivideAndConquer {
 public:
  EmployeesAlgorithm(const std::vector<unsigned int>& free_days, int total_days, const SatisfactionMatrix& satisfaction_rates)
      : free_days_(free_days), total_days_(total_days), satisfaction_rates_(satisfaction_rates) {}
 private:
  bool Small(const Instance* instance) override;
  std::unique_ptr<Solution> SolveSmall(const Instance* instance) override;
  std::vector<std::unique_ptr<Instance>> Divide(const Instance* instance) override;
  std::unique_ptr<Solution> Combine(std::vector<std::unique_ptr<Solution>> partial_solutions) override;

  SatisfactionMatrix satisfaction_rates_;
  std::vector<unsigned int> free_days_;
  int total_days_;
};
 
#endif // EMPLOYEES_ALGORITHM_H