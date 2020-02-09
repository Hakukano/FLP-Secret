#ifndef SECRET_SECRET_HPP
#define SECRET_SECRET_HPP

#include <optional>
#include <string>
#include <vector>

namespace secret {
class Secret {
 public:
  static constexpr char kDataExtension[]{".flpsdata"};
  static constexpr char kCodeExtension[]{".flpscode"};
  static constexpr char kDefaultOutPath[]{"./out/"};

  Secret(const std::string& data_path, const std::optional<std::string>& code_path = std::nullopt);

  void Encrypt();
  void Decrypt();

  void Save(bool encrypt, const std::optional<std::string>& out_dir_path);

 private:
  static constexpr std::size_t kCodeLengthMin{1000};
  static constexpr std::size_t kCodeLengthMax{2000};

  std::string data_stem_;
  std::string data_ext_;
  std::vector<char> data_;
  std::vector<char> code_;
};
}  // namespace secret

#endif
