#include <filesystem>
#include <iostream>
#include <string>

#include "secret.hpp"
#include "util/fs/fio.hpp"
#include "util/lang/trie.hpp"
#include "util/math/cmathenhance.hpp"

static void Usage() {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "\tsecret e <in_file_path> [out_dir_path]" << std::endl;
  std::cerr << "\tsecret ed <in_file_dir_path> [out_dir_path]" << std::endl;
  std::cerr << "\tsecret d <in_file_path> <in_code_path> [out_dir_path]" << std::endl;
  std::cerr << "\tsecret dd <in_file_dir_path> <in_code_dir_path> [out_dir_path]" << std::endl;
  exit(1);
}

int main(int args, char* argv[]) {
  if (!(flp_util::IsInTheRange(args, 3, 5))) {
    Usage();
  }

  const std::string& cmd{std::string(argv[1])};
  try {
    if (cmd == "e" && flp_util::IsInTheRange(args, 3, 4)) {
      const std::string& in_file_path{argv[2]};
      secret::Secret s{in_file_path};
      s.Encrypt();
      std::optional<std::string> out_dir_path{};
      if (args == 4) {
        out_dir_path = argv[3];
      }
      s.Save(true, out_dir_path);
    } else if (cmd == "ed" && flp_util::IsInTheRange(args, 3, 4)) {
      for (const auto& in_file_path : flp_util::GetAllFiles(argv[2])) {
        secret::Secret s{in_file_path};
        s.Encrypt();
        std::optional<std::string> out_dir_path{};
        if (args == 4) {
          out_dir_path = argv[3];
        }
        s.Save(true, out_dir_path);
      }
    } else if (cmd == "d" && flp_util::IsInTheRange(args, 4, 5)) {
      const std::string& in_file_path{argv[2]};
      const std::string& in_code_path{argv[3]};
      secret::Secret s{in_file_path, in_code_path};
      s.Decrypt();
      std::optional<std::string> out_dir_path{};
      if (args == 5) {
        out_dir_path = argv[4];
      }
      s.Save(false, out_dir_path);
    } else if (cmd == "dd" && flp_util::IsInTheRange(args, 4, 5)) {
      flp_util::Trie<std::string> in_code_trie{};
      for (const auto& in_code_path : flp_util::GetAllFiles(argv[3])) {
        std::optional<std::string> code_ext{flp_util::FileExtension(in_code_path)};
        if (!(code_ext && *code_ext == secret::Secret::kCodeExtension)) {
          continue;
        }
        std::optional<std::string> code_stem{flp_util::FileStem(in_code_path)};
        if (!code_stem) {
          continue;
        }
        in_code_trie.Add(*code_stem, in_code_path);
      }
      for (const auto& in_file_path : flp_util::GetAllFiles(argv[2])) {
        std::optional<std::string> file_ext{flp_util::FileExtension(in_file_path)};
        if (!(file_ext && *file_ext == secret::Secret::kDataExtension)) {
          continue;
        }
        std::optional<std::string> file_stem{flp_util::FileStem(in_file_path)};
        if (!file_stem) {
          continue;
        }
        const std::string* in_code_path{in_code_trie.Search(*file_stem)};
        if (!in_code_path) {
          continue;
        }
        secret::Secret s{in_file_path, *in_code_path};
        s.Decrypt();
        std::optional<std::string> out_dir_path{};
        if (args == 5) {
          out_dir_path = argv[4];
        }
        s.Save(false, out_dir_path);
      }
    } else {
      Usage();
    }
  } catch (flp_util::FIO_Status err) {
    std::cerr << "Error " << static_cast<int>(err) << ": Cannot read file" << std::endl;
  }
  return 0;
}
