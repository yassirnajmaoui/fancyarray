#include "StructOfVectors.hpp"
#include "VectorOfStructs.hpp"

#include <iostream>

template <typename V>
void printContents(V v)
{
	if (v.size() == 0)
		return;
	std::cout << "Size of type: " << sizeof(v[0]) << std::endl;
	for (const auto& val : v)
	{
		std::cout << val << std::endl;
	}
	std::cout << "---------" << std::endl;
}

int main(int, char**)
{
	StructOfVectors<int, double, float> sa;
	sa.get<0>() = {15, 24, 58, 69};
	sa.get<1>() = {3.14159, 2.5, 9.5, 2.999};
	sa.get<2>() = {99.14159, 202.5, 0.1, 0.859};
	sa.save("file1");

	StructOfVectors<int, double, float> sb;
	sb.read("file1");
	printContents(sb.get<0>());
	printContents(sb.get<1>());
	printContents(sb.get<2>());
	std::cout << std::endl;

	VectorOfStructs<int, double, float> va;
	va.get().resize(5);
	for (size_t i = 0; i < 5; i++)
	{
		va.set<0>(i, i);
		va.set<1>(i, i / 3.2);
		va.set<2>(i, i / 4.5f);
	}
	va.save("file2");

	VectorOfStructs<int, double, float> vb;
	vb.read("file2");
	std::cout << std::endl;
}
