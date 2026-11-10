/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Diseño y Análisis de Algoritmos
 * Curso: 3º
 * Práctica 2: Simulador de la Máquina RAM
 * Autores: Manuel Cadenas García alu0101636849@ull.edu.es
 *          Guillermo López Concepción alu0101620459@ull.edu.es
 * Fecha: 12/02/2026
 */

#ifndef VECTOR_OPERAND_H
#define VECTOR_OPERAND_H

#include "operand.h"
#include "RAM_Machine.h"

/**
 * @class VectorOperand
 * @brief Represents a vector operand in the RAM machine, which uses a register index as a base index and a fixed internal index to access memory locations in a vector-like manner.
 */
class VectorOperand : public Operand {
public:
	VectorOperand(int base_index, int internal_index) : 
	              base_index_{base_index}, internal_index_{internal_index} {}

	int getValue(RAMMachine& machine) const override {
		return machine.getMemory().read(base_index_, internal_index_);
	}

	void setValue(RAMMachine& machine, int value) const override {
		machine.getMemory().write(base_index_, value, internal_index_);
	}

	bool resolvesToR0(RAMMachine& machine) const override {
		return base_index_ == 0;
	}
private:
	int base_index_;
	int internal_index_;
};

#endif // VECTOR_OPERAND_H