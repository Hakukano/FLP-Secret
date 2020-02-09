#include "secret.hpp"

#include <climits>
#include <filesystem>

#include "util/fs/fio.hpp"
#include "util/math/cmathenhance.hpp"
#include "util/math/randomenhance.hpp"
#include "version.hpp"

secret::Secret::Secret(const std::string& data_path, const std::optional<std::string>& code_path) {
  if (code_path && !(VersionMatch(data_path) && VersionMatch(*code_path))) {
    throw flp_util::FIO_Status::UNKNOWN;
  }

  std::optional<std::string> data_stem{flp_util::FileStem(data_path)};
  if (!data_stem) {
    throw flp_util::FIO_Status::UNKNOWN;
  }
  if (code_path) {
    data_stem = flp_util::FileStem(*data_stem);
    if (!data_stem) {
      throw flp_util::FIO_Status::UNKNOWN;
    }
  }
  data_stem_ = *data_stem;

  std::optional<std::string> data_ext{flp_util::FileExtension(data_path)};
  data_ext_ = data_ext ? *data_ext : "";

  if (flp_util::ReadFileAsByteArray(data_path, data_) != flp_util::FIO_Status::OK) {
    throw flp_util::FIO_Status::FILE_NOT_EXISTS;
  }

  if (code_path) {
    if (flp_util::ReadFileAsByteArray(*code_path, code_) != flp_util::FIO_Status::OK) {
      throw flp_util::FIO_Status::FILE_NOT_EXISTS;
    }
  } else {
    const std::size_t code_length{flp_util::RandomUniformInt<std::size_t>(1, {kCodeLengthMin, kCodeLengthMax}).at(0)};
    const std::vector<int> code_random{flp_util::RandomUniformInt<int>(code_length, {SCHAR_MIN, SCHAR_MAX})};
    code_.clear();
    code_.reserve(code_length);
    for (auto c : code_random) {
      code_.emplace_back(static_cast<char>(c));
    }
  }
}

void secret::Secret::Encrypt() {
  for (std::size_t i{0}; i < data_.size(); ++i) {
    data_.at(i) =
        static_cast<char>(
            flp_util::LoopIntoTheRange(
                static_cast<int>(data_.at(i)) +
                    static_cast<int>(code_.at(i % code_.size())),
                SCHAR_MIN, SCHAR_MAX));
  }
}

void secret::Secret::Decrypt() {
  for (std::size_t i{0}; i < data_.size(); ++i) {
    data_.at(i) =
        static_cast<char>(
            flp_util::LoopIntoTheRange(
                static_cast<int>(data_.at(i)) -
                    static_cast<int>(code_.at(i % code_.size())),
                SCHAR_MIN, SCHAR_MAX));
  }
}

void secret::Secret::Save(bool encrypt, const std::optional<std::string>& out_dir_path) {
  std::filesystem::path op{kDefaultOutPath};
  if (out_dir_path) {
    op = {*out_dir_path};
  }
  if (!std::filesystem::exists(op)) {
    std::filesystem::create_directories(op);
  }
  if (encrypt) {
    flp_util::WriteByteArrayToFile((op / (data_stem_ + data_ext_ + "." + kVersion + kDataExtension)).string(), data_);
    flp_util::WriteByteArrayToFile((op / (data_stem_ + data_ext_ + "." + kVersion + kCodeExtension)).string(), code_);
  } else {
    flp_util::WriteByteArrayToFile((op / data_stem_).string(), data_);
  }
}
