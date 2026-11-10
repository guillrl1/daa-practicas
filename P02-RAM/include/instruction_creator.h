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

#ifndef INSTRUCTION_CREATOR_H
#define INSTRUCTION_CREATOR_H

#include "RAM_Machine.h"
#include "operand_creator.h"

#include "load_instruction.h"
#include "store_instruction.h"
#include "jump_instruction.h"
#include "jgtz_instruction.h"
#include "jzero_instruction.h"
#include "read_instruction.h"
#include "write_instruction.h"
#include "add_instruction.h"
#include "sub_instruction.h"
#include "mul_instruction.h"
#include "div_instruction.h"
#include "halt_instruction.h"
#include "jltz_instruction.h"

#include "module_instruction.h"

#include <map>
#include <string>
#include <memory>
#include <functional>
#include <stdexcept>

/**
 * @class InstructionCreator
 * @brief Factory class responsible for creating Instruction instances based on 
 *        opcode and operand strings.
 */
class InstructionCreator {
 public:
  using Builder = std::function<std::unique_ptr<Instruction>(const std::string&)>;

  /**
   * @brief Creates an Instruction instance based on the given opcode and operand string.
   *        It uses a registry of opcode to builder functions to determine how to construct the instruction.
   * @param op_code The opcode of the instruction (e.g., "LOAD", "ADD", "JUMP").
   * @param operand_str The string representation of the operand, which will be parsed by the OperandCreator if needed.
   * @return A unique pointer to the created Instruction instance.
   * @throws std::invalid_argument if the opcode is unknown or if there are issues with the operand format.
   * @throws std::runtime_error if there are errors during instruction creation (e.g., invalid operand).
   */
  static std::unique_ptr<Instruction> createInstruction(const std::string& op_code, 
														                            const std::string& operand_str) {
		static const std::map<std::string, Builder> registry = {
			{"LOAD", [](const std::string& operand_str) {
				return std::make_unique<LoadInstruction>(OperandCreator::create(operand_str));
			}},
			{"HALT", [](const std::string& operand_str) {
				return std::make_unique<HaltInstruction>();
			}},
			{"STORE", [](const std::string& operand_str) {
				return std::make_unique<StoreInstruction>(OperandCreator::create(operand_str));
			}},
			{"JUMP", [](const std::string& operand_str) {
				return std::make_unique<JumpInstruction>(operand_str);
			}},
			{"JGTZ", [](const std::string& operand_str) {
				return std::make_unique<JgtzInstruction>(operand_str);
			}},
			{"JZERO", [](const std::string& operand_str) {
				return std::make_unique<JzeroInstruction>(operand_str);
			}},
			{"READ", [](const std::string& operand_str) {
				return std::make_unique<ReadInstruction>(OperandCreator::create(operand_str));
			}},
			{"WRITE", [](const std::string& operand_str) {
				return std::make_unique<WriteInstruction>(OperandCreator::create(operand_str));
			}},
			{"ADD", [](const std::string& operand_str) {
				return std::make_unique<AddInstruction>(OperandCreator::create(operand_str));
			}},
			{"SUB", [](const std::string& operand_str) {
				return std::make_unique<SubInstruction>(OperandCreator::create(operand_str));
			}},
			{"MUL", [](const std::string& operand_str) {
				return std::make_unique<MulInstruction>(OperandCreator::create(operand_str));
			}},
			{"DIV", [](const std::string& operand_str) {
				return std::make_unique<DivInstruction>(OperandCreator::create(operand_str));
			}},
			{"JLTZ", [](const std::string& operand_str) {
				return std::make_unique<JltzInstruction>(operand_str);
			}},
			{"MOD", [](const std::string& operand_str) {
				return std::make_unique<ModuleInstruction>(OperandCreator::create(operand_str));
			}}
		};
		auto it = registry.find(op_code);
		if (it != registry.end()) {
			return it->second(operand_str);
		} else {
			throw std::invalid_argument("Error fatal de carga: Instrucción desconocida: " + op_code);
		}
  }
};

#endif // INSTRUCTION_CREATOR_H