#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

template <typename F, typename... Args>
void call_elem(F f, Args... args) {
  int x[] = {(f(args), 0)...};
}

template <typename Functor, typename... Args>
void call_elems(Functor func, Args... args) {
  call_elem([&](const auto& arg) { func(arg); }, args...);
}

template <typename Functor, typename Tuple, size_t... Is>
void call_elems_in_tuple_i(Functor func, Tuple t, std::index_sequence<Is...>) {
  call_elems(func, std::get<Is>(t)...);
}

template <typename Functor, typename... Types>
void call_elems_in_tuple(Functor func, std::tuple<Types...> t) {
  constexpr size_t N = std::tuple_size<decltype(t)>::value;
  std::make_index_sequence<N> a;
  call_elems_in_tuple_i(func, t, a);
}

template <typename... Types>
class StructOfVectors {
  typedef std::tuple<std::vector<Types>...> vtype;
  vtype vectors;

 public:
  template <size_t row>
  constexpr auto& get() {
    return std::get<row>(vectors);
  }

  void save(std::string fname) {
    std::ofstream file;
    file.open(fname.c_str(), std::ios::binary | std::ios::out);
    if (!file.is_open()) {
      throw std::system_error(
          std::make_error_code(std::errc::no_such_file_or_directory),
          "Unable to open file");
    }
    call_elems_in_tuple(
        [&file]<typename T>(const std::vector<T>& v) {
          file.write((char*)(v.data()), v.size() * sizeof(T));
        },
        vectors);
  }
};