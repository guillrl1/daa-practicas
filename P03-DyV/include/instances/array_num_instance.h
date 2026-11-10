/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 25/02/2026
 */

#ifndef ARRAY_NUM_INSTANCE_H
#define ARRAY_NUM_INSTANCE_H

#include "instance.h"

#include <vector>

/**
 * @class ArrayNumInstance
 * @brief Concrete class representing a specific instance of a problem where the
 *        instance consists of a vector of integers that needs to be sorted. It
 *        inherits from the abstract base class Instance and provides a constructor
 *        to initialize the vector of integers.
 */
class ArrayNumInstance : public Instance {
 public:
  ArrayNumInstance(const std::vector<int>& numbers) : numbers_(numbers) {}
  const std::vector<int>& GetNumbers() const { return numbers_; }
 private:
  std::vector<int> numbers_;
};

#endif // ARRAY_NUM_INSTANCE_H