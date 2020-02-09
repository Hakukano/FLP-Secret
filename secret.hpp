#ifndef SECRET_SECRET_HPP
#define SECRET_SECRET_HPP

#include <optional>
#include <string>
#include <vector>

namespace secret {
class Secret {
 public:
  Secret(const std::string& data_path, const std::optional<std::string>& code_path = std::nullopt);

  void Encode();
  void Decode();

  void Save(const std::string& data_path, const std::optional<std::string>& code_path = std::nullopt);

 private:
  static constexpr std::size_t kCodeLengthMin{1};
  static constexpr std::size_t kCodeLengthMax{100};

  std::vector<char> data_;
  std::vector<char> code_;
};
}  // namespace secret

#endif
