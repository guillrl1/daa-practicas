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

#ifndef RAM_MACHINE_H
#define RAM_MACHINE_H

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

#include "data_memory.h"
#include "instruction.h"
#include "input_tape.h"
#include "output_tape.h"

/**
 * @class RAMMachine
 * @brief Simulates a RAM (Random Access Machine) with program memory, 
 *        data memory, input tape, and output tape.
 */
class RAMMachine {
 public:
  /**
   * @brief Constructs a RAMMachine with the given input tape and output file.
   * @param input_tape A vector of integers representing the input tape values.
   * @param output_file The name of the file where the output tape will be flushed.
   */
  RAMMachine(const std::vector<int>& input_tape, const std::string& output_file)
      : pc_(0),
        running_(true),
        instructions_count_(0),
        input_tape_(input_tape),
        output_tape_(output_file) {}

  // ── Program loading ───────────────────────────────────────────────────────

  /**
   * @brief Adds an instruction to the program memory.
   * @param inst A unique pointer to the instruction to be added.
   */
  void addInstruction(std::unique_ptr<Instruction> inst) {
    program_memory_.push_back(std::move(inst));
  }

  /**
   * @brief Adds a label to the symbol table, associating it with the current
   *        program memory size (the address of the next instruction).
   * @param label The label name to be added.
   */
  void addLabel(const std::string& label) {
    symbol_table_.emplace_back(label, program_memory_.size());
  }

  // ── Execution ─────────────────────────────────────────────────────────────

  /**
   * @brief Executes the loaded program until a HALT instruction is encountered or
   *        the program counter exceeds the program memory size.
   *        Increments the instruction count for each executed instruction.
   */
  void run() {
    while (running_ && pc_ < program_memory_.size()) {
      program_memory_[pc_++]->execute(*this);
      ++instructions_count_;
    }
  }

  // ── State control ─────────────────────────────────────────────────────────

  void setPC(size_t val)   { pc_ = val; }
  void halt()              { running_ = false; }

  // ── Accessors ─────────────────────────────────────────────────────────────

  DataMemory&  getMemory()      { return data_memory_; }
  InputTape&   getInputTape()   { return input_tape_; }
  OutputTape&  getOutputTape()  { return output_tape_; }
  size_t       getPc()    const { return pc_; }
  long         getCount() const { return instructions_count_; }

  /**
   * @brief Retrieves the program memory address associated with a given label.
   * @param label The label name to look up in the symbol table.
   * @return The program memory address associated with the label.
   * @throws std::runtime_error if the label is not found in the symbol table.
   */
  size_t getLabelAddress(const std::string& label) const {
    for (const auto& entry : symbol_table_) {
      if (entry.first == label) {
        return entry.second;
      }
    }
    throw std::runtime_error("Label not found: " + label);
  }

  /**
   * @brief Flushes the output tape, writing all accumulated values to the output file.
   *        This is typically called automatically on HALT or on error to ensure that all output is saved.
   */
  void flushOutputTape() {
    output_tape_.flush();
  }

  // ── Debug helpers ─────────────────────────────────────────────────────────

  /**
   * @brief Prints the symbol table to the standard output for debugging purposes.
   */
  void printSymbolTable() const {
    std::cout << "Symbol Table:\n";
    for (const auto& entry : symbol_table_) {
      std::cout << "  " << entry.first << " -> " << entry.second << "\n";
    }
  }

  /**
   * @brief Prints the program memory size (number of instructions) to the standard output for debugging purposes.
   */
  void printProgram() const {
    std::cout << "Program Memory: " << program_memory_.size() << " instructions\n";
  }

  /**
   * @brief Prints the contents of the data memory to the standard output for debugging purposes.
   *        Shows each register and its internal indices with their corresponding values.
   */
  void printDataMemory() const {
    std::cout << "Data Memory:\n";
    for (const auto& reg : data_memory_.getRegisters()) {
      std::cout << "  R" << reg.first << ": ";
      for (size_t i = 0; i < reg.second.size(); ++i) {
        std::cout << "[" << i << "]=" << reg.second[i] << " ";
      }
      std::cout << "\n";
    }
  }
 private:
  DataMemory   data_memory_;
  InputTape    input_tape_;
  OutputTape   output_tape_;

  std::vector<std::unique_ptr<Instruction>> program_memory_;
  std::vector<std::pair<std::string, size_t>> symbol_table_;

  size_t pc_;
  bool   running_;
  long   instructions_count_;
};

#endif // RAM_MACHINE_H