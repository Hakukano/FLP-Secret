#include <filesystem>
#include <iostream>
#include <string>

#include "secret.hpp"
#include "util/fs/fio.hpp"

int main(int args, char* argv[]) {
  if (!(args == 5 && (std::string(argv[1]) == "e" || std::string(argv[1]) == "d"))) {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "\tsecret e <in_file_path> <out_file_path> <out_code_path>" << std::endl;
    std::cerr << "\tsecret d <in_file_path> <in_code_path> <out_file_path>" << std::endl;
    return 1;
  }

  const std::string& cmd{std::string(argv[1])};
  try {
    if (cmd == "e") {
      const std::string& in_file_path{argv[2]};
      const std::string& out_file_path{argv[3]};
      const std::string& out_code_path{argv[4]};
      secret::Secret s{in_file_path};
      s.Encode();
      s.Save(out_file_path, out_code_path);
    } else if (cmd == "d") {
      const std::string& in_file_path{argv[2]};
      const std::string& in_code_path{argv[3]};
      const std::string& out_file_path{argv[4]};
      secret::Secret s{in_file_path, in_code_path};
      s.Decode();
      s.Save(out_file_path);
    }
  } catch (flp_util::FIO_Status err) {
    std::cerr << "Error " << static_cast<int>(err) << ": Cannot read file" << std::endl;
  }
  return 0;
}
