#include "version.hpp"

#include "util/fs/fio.hpp"

bool secret::VersionMatch(const std::string& path) {
  std::optional<std::string> stem{flp_util::FileStem(path)};
  if (!stem) {
    return false;
  }
  std::optional<std::string> ext{flp_util::FileExtension(*stem)};
  return (ext && ext == std::string(".") + kVersion);
}
