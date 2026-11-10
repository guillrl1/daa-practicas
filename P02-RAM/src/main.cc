/**
 * @file main.cc
 * @brief Entry point for the RAM machine simulator.
 *
 * Usage:
 *   ./ram_simulator <program.ram> <input_tape.txt> <output_tape.txt>
 *
 *   program.ram      RAM program to execute.
 *   input_tape.txt   One integer per line (or space-separated) for the input tape.
 *   output_tape.txt  File where the output tape is written after execution or error.
 */

#include "program_loader.h"
#include "RAM_Machine.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

/**
 * @brief Reads all integers from a file into a vector.
 * @throws std::runtime_error if the file cannot be opened.
 */
static std::vector<int> loadInputTape(const std::string& file_name) {
  std::ifstream file(file_name);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open input tape file: " + file_name);
  }
  std::vector<int> tape;
  int value;
  while (file >> value) {
    tape.push_back(value);
  }
  return tape;
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <program.ram> <input_tape.txt> <output_tape.txt>\n";
    return EXIT_FAILURE;
  }

  const std::string program_file = argv[1];
  const std::string input_file   = argv[2];
  const std::string output_file  = argv[3];

  // Declare machine outside try so it is accessible in the catch block,
  // allowing us to flush whatever output was produced before the error.
  try {
    const std::vector<int> input_data = loadInputTape(input_file);
    RAMMachine machine(input_data, output_file);

    ProgramLoader::loadProgram(program_file, machine);

    try {
      machine.run();
    } catch (const std::exception& e) {
      // Runtime error during execution: report it, flush partial output, exit.
      std::cerr << "Runtime error: " << e.what() << "\n";
      machine.flushOutputTape();
      std::cerr << "Partial output written to: " << output_file << "\n";
      return EXIT_FAILURE;
    }

    machine.flushOutputTape();
    std::cout << "Execution finished. Instructions executed: "
              << machine.getCount() << "\n";
    std::cout << "Output written to: " << output_file << "\n";

  } catch (const std::exception& e) {
    // Load-time error (bad file, syntax error, unknown instruction…)
    std::cerr << "Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}