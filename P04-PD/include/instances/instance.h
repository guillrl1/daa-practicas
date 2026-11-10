/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 3: Experimentación con Divide y Vencerás
 * Autores: Manuel Cadenas García alumno
 *          Guillermo López Concepción guillrl1@users.noreply.github.com
 * Fecha: 25/02/2026
 */

#ifndef INSTANCE_H
#define INSTANCE_H

/**
 * @class Instance
 * @brief Abstract base class representing a general instance of a problem to 
 *        be solved by an algorithm.
 */
class Instance {
 public:
  virtual ~Instance() = default;
};

#endif // INSTANCE_H