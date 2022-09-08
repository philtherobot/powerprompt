#pragma once

#include <iostream>
#include <variant>

template<class T>
struct streamer {
  const T& val;
};
template<class T> streamer(T) -> streamer<T>;

template<class... Ts>
std::ostream & operator<<(std::ostream & os, streamer<std::variant<Ts...>> sv) {
  std::visit([&os](const auto& v) { os << v; }, sv.val);
  return os;
}
