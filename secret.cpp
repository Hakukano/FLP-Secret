#include "secret.hpp"

#include <climits>

#include "util/fs/fio.hpp"
#include "util/math/cmathenhance.hpp"
#include "util/math/randomenhance.hpp"

secret::Secret::Secret(const std::string& data_path, const std::optional<std::string>& code_path) {
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

void secret::Secret::Encode() {
  for (std::size_t i{0}; i < data_.size(); ++i) {
    data_.at(i) =
        static_cast<char>(
            flp_util::LoopIntoTheRange(
                static_cast<int>(data_.at(i)) +
                    static_cast<int>(code_.at(i % code_.size())),
                SCHAR_MIN, SCHAR_MAX));
  }
}

void secret::Secret::Decode() {
  for (std::size_t i{0}; i < data_.size(); ++i) {
    data_.at(i) =
        static_cast<char>(
            flp_util::LoopIntoTheRange(
                static_cast<int>(data_.at(i)) -
                    static_cast<int>(code_.at(i % code_.size())),
                SCHAR_MIN, SCHAR_MAX));
  }
}

void secret::Secret::Save(const std::string& data_path, const std::optional<std::string>& code_path) {
  flp_util::WriteByteArrayToFile(data_path, data_);
  if (code_path) {
    flp_util::WriteByteArrayToFile(*code_path, code_);
  }
}
