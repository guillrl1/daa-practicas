/**
 * @file benchmark_sort.cc
 * @brief Benchmark: Insertion Sort vs BubbleSort en la máquina RAM.
 *
 * Genera los inputs en ficheros temporales, ejecuta el simulador
 * y muestra una tabla comparativa de instrucciones ejecutadas.
 *
 * Compilar:
 *   g++ -std=c++20 -O2 -o benchmark_sort benchmark_sort.cc
 *
 * Uso:
 *   ./benchmark_sort
 *
 * Estructura esperada:
 *   bin/p01
 *   Ejemplos-Sort/insertion_sort.ram
 *   Ejemplos-Sort/bubble_sort.ram
 */

#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

// ── configuration ─────────────────────────────────────────────────────────────

static const std::string BINARY    = "./bin/p01";
static const std::string INSERTION = "Ejemplos-Sort/insertion_sort.ram";
static const std::string BUBBLE    = "Ejemplos-Sort/bubble_sort.ram";
static const std::vector<int> SIZES = {50, 100, 500, 1000};

// ── helpers ───────────────────────────────────────────────────────────────────

/**
 * @brief Writes a descending input tape of size n to a temp file.
 * @returns the path of the created file.
 */
static std::string makeInput(int n) {
  // Use tmpnam for simplicity (acceptable in a benchmark tool)
  char tmp_path[L_tmpnam];
  std::tmpnam(tmp_path);
  std::string path(tmp_path);
  path += ".txt";

  std::ofstream f(path);
  f << n;
  for (int i = n; i >= 1; --i) {
    f << ' ' << i;
  }
  f << '\n';
  return path;
}

/**
 * @brief Runs the simulator and extracts the instruction count.
 * @returns the count, or std::nullopt on error.
 */
static std::optional<long> runSimulator(const std::string& ram,
                                        const std::string& input_path,
                                        std::string& error_out) {
  // Temp file for the output tape
  char tmp_out[L_tmpnam];
  std::tmpnam(tmp_out);
  std::string out_path(tmp_out);
  out_path += ".txt";

  // Build the command and capture stdout+stderr
  std::string cmd = BINARY + " " + ram + " " + input_path + " " + out_path + " 2>&1";

  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    error_out = "popen failed";
    return std::nullopt;
  }

  std::string output;
  std::array<char, 256> buf;
  while (fgets(buf.data(), buf.size(), pipe)) {
    output += buf.data();
  }
  int ret = pclose(pipe);
  std::remove(out_path.c_str());

  if (ret != 0) {
    // Return first line of output as error message
    error_out = output.substr(0, output.find('\n'));
    return std::nullopt;
  }

  // Parse "Instructions executed: NNNN"
  const std::string marker = "Instructions executed: ";
  const size_t pos = output.find(marker);
  if (pos == std::string::npos) {
    error_out = "No instruction count in output";
    return std::nullopt;
  }
  return std::stol(output.substr(pos + marker.size()));
}

/**
 * @brief Formats a number with thousands separators (e.g. 1234567 → "1,234,567").
 */
static std::string fmt(long n) {
  std::string s = std::to_string(n);
  int insert_pos = static_cast<int>(s.size()) - 3;
  while (insert_pos > 0) {
    s.insert(insert_pos, ",");
    insert_pos -= 3;
  }
  return s;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
  // Check that required files exist
  for (const auto& path : {BINARY, INSERTION, BUBBLE}) {
    std::ifstream f(path);
    if (!f.good()) {
      std::cerr << "ERROR: no se encuentra " << path << "\n";
      return EXIT_FAILURE;
    }
  }

  // Header
  std::cout << "\n";
  std::cout << "Benchmark: Insertion Sort vs BubbleSort (peor caso — entrada descendente)\n";
  std::cout << std::string(70, '-') << "\n";
  std::cout
    << std::right
    << std::setw(6)  << "n"
    << std::setw(18) << "Insertion Sort"
    << std::setw(18) << "Bubble Sort"
    << std::setw(12) << "Ratio B/I"
    << "\n";
  std::cout
    << std::setw(6)  << "------"
    << std::setw(18) << "--------------"
    << std::setw(18) << "-----------"
    << std::setw(12) << "---------"
    << "\n";

  for (int n : SIZES) {
    const std::string input_path = makeInput(n);

    std::string err;
    const auto ins = runSimulator(INSERTION, input_path, err);
    const auto bub = runSimulator(BUBBLE,    input_path, err);

    std::remove(input_path.c_str());

    const std::string ins_str = ins ? fmt(*ins) : "ERROR";
    const std::string bub_str = bub ? fmt(*bub) : "ERROR";
    const std::string ratio   = (ins && bub)
      ? ([&]{ std::ostringstream ss; ss.precision(2);
              ss << std::fixed << (double)*bub / *ins << "x"; return ss.str(); }())
      : "N/A";

    std::cout
      << std::setw(6)  << n
      << std::setw(18) << ins_str
      << std::setw(18) << bub_str
      << std::setw(12) << ratio
      << "\n";
  }

  std::cout << "\n";
  return EXIT_SUCCESS;
}