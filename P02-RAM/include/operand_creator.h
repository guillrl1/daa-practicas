#ifndef OPERAND_FACTORY_H
#define OPERAND_FACTORY_H

#include <memory>
#include <string>
#include <regex>
#include <stdexcept>

#include "operand.h"
#include "immediate_operand.h"
#include "direct_operand.h"
#include "indirect_operand.h"
#include "vector_operand.h"
#include "dynamic_vector_operand.h"

/**
 * @class OperandCreator
 * @brief Factory class responsible for creating Operand instances based on string representations.
 *        It parses the operand string and determines the appropriate Operand subclass to instantiate.
 */
class OperandCreator {
 public:
  static std::unique_ptr<Operand> create(const std::string& str) {
    if (str.empty()) {
      throw std::invalid_argument("Operand string is empty");
    }

    // 1. Immediate (=NUMBER)
    if (str[0] == '=') {
      return std::make_unique<ImmediateOperand>(std::stoi(str.substr(1)));
    }

    // 2. Indirect (*NUMBER)
    if (str[0] == '*') {
      return std::make_unique<IndirectOperand>(std::stoi(str.substr(1)));
    }

    // 3. Dynamic vector (NUMBER[R NUMBER])
    const std::regex dynamic_vector_regex(R"((\d+)\[R(\d+)\])");
    std::smatch dyn_match;
    if (std::regex_match(str, dyn_match, dynamic_vector_regex)) {
      const int base_reg = std::stoi(dyn_match[1]);
      const int index_reg = std::stoi(dyn_match[2]);
      return std::make_unique<DynamicVectorOperand>(base_reg, index_reg);
    }

    // 4. Static vector (NUMBER[NUMBER])
    const std::regex static_vector_regex(R"((\d+)\[(\d+)\])");
    std::smatch static_match;
    if (std::regex_match(str, static_match, static_vector_regex)) {
      const int index = std::stoi(static_match[1]);
      const int internal_index = std::stoi(static_match[2]);
      return std::make_unique<VectorOperand>(index, internal_index);
    }

    // 5. Direct (NUMBER)
    try {
      return std::make_unique<DirectOperand>(std::stoi(str));
    } catch (const std::invalid_argument&) {
      throw std::invalid_argument("Formato inválido de operando: " + str);
    }
  }
};

#endif // OPERAND_FACTORY_H