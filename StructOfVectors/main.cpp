#include "StructOfVectors.hpp"

template <typename V>
void printContents(V v) {
  if (v.size() == 0) return;
  std::cout << "Size of type: " << sizeof(v[0]) << std::endl;
  for (const auto &val : v) {
    std::cout << val << std::endl;
  }
  std::cout << "---------" << std::endl;
}

int main(int, char **) {
  StructOfVectors<int, double, float> sa;
  sa.get<0>() = {15, 24, 58, 69};
  sa.get<1>() = {3.14159, 2.5};
  sa.get<2>() = {99.14159, 202.5};
  printContents(sa.get<0>());
  printContents(sa.get<1>());
  printContents(sa.get<2>());
  sa.save("file");
}
