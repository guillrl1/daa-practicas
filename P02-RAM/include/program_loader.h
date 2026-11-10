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

#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include <stdexcept>
#include <fstream>
#include <string>
#include <regex>
#include <set>

#include "RAM_Machine.h"
#include "instruction_creator.h"

/**
 * @class ProgramLoader
 * @brief Responsible for loading a RAM program from a .ram file into a 
 *        RAMMachine instance
 */
class ProgramLoader {
 public:

  /**
   * @brief Loads a RAM program from the specified file into the given RAMMachine.
   *        Parses the file line by line, handling labels, instructions, and
   *        comments. Validates syntax and instruction formats, and throws
   *        exceptions with descriptive error messages if any issues are encountered.
   * @param file_name The name of the .ram file containing the program to load.
   * @param machine The RAMMachine instance into which the program will be loaded.
   * @throws std::invalid_argument if the file name is empty or does not have a .ram extension.
   * @throws std::runtime_error if the file cannot be opened or if there are syntax errors in the program.
   */
  static void loadProgram(const std::string& file_name, RAMMachine& machine) {
    if (file_name.empty()) {
      throw std::invalid_argument("File name cannot be empty");
    }
    if (file_name.size() < 4 || file_name.substr(file_name.size() - 4) != ".ram") {
      throw std::invalid_argument("File must have .ram extension");
    }

    std::ifstream file(file_name);
    if (!file.is_open()) {
      throw std::runtime_error("Cannot open program file: " + file_name);
    }

    // Jump instructions whose operand is a label (must be uppercased)
    static const std::set<std::string> jump_opcodes = {"JUMP", "JZERO", "JGTZ"};

    const std::regex line_regex(R"(^\s*(?:(\w+):)?\s*(\w+)?\s*(\S*)?\s*(?:#.*)?$)");

    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
      ++line_number;

      const std::string trimmed = trim(line);
      if (trimmed.empty() || trimmed[0] == '#') {
        continue;
      }

      std::smatch match;
      if (!std::regex_match(line, match, line_regex)) {
        throw std::runtime_error(
          "Line " + std::to_string(line_number) + ": invalid syntax: " + line);
      }

      std::string label   = toUpper(match[1]);
      std::string op_code = toUpper(match[2]);
      std::string operand = match[3];

      if (jump_opcodes.count(op_code)) {
        operand = toUpper(operand);
      }

      if (!label.empty()) {
        machine.addLabel(label);
      }

      if (!op_code.empty()) {
        try {
          auto instruction = InstructionCreator::createInstruction(op_code, operand);
          machine.addInstruction(std::move(instruction));
        } catch (const std::exception& e) {
          throw std::runtime_error(
            "Line " + std::to_string(line_number) + ": " + e.what());
        }
      }
    }
  }
 private:

  /**
   * @brief Converts a string to uppercase.
   * @param s The input string to convert.
   * @return A new string that is the uppercase version of the input string.
   */
  static std::string toUpper(const std::string& s) {
    std::string result = s;
    for (char& c : result) c = static_cast<char>(std::toupper(c));
    return result;
  }

  /**
   * @brief Trims leading and trailing whitespace from a string.
   * @param s The input string to trim.
   * @return A new string with leading and trailing whitespace removed.
   */
  static std::string trim(const std::string& s) {
    const size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
  }
};

#endif // PROGRAM_LOADER_H