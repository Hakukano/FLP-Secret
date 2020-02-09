#ifndef SECRET_VERSION_HPP
#define SECRET_VERSION_HPP

#include <string>

namespace secret {
inline constexpr const char kVersion[]{"v1_1_0"};

bool VersionMatch(const std::string& path);
}
#endif
