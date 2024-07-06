#include "StructOfVectors.hpp"
#include "TrivialArrayOfStructs.hpp"
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

	VectorOfStructs<float, unsigned int, unsigned int> vb;
	vb.read("file2");
	std::cout << std::endl;

	TrivialArrayOfStructs<int, double, unsigned char, unsigned long> s{3};
	std::cout << "size of:" << decltype(s)::get_sizeof<1>() << std::endl;
	std::cout << "total size of:" << decltype(s)::get_size_per_row()
			  << std::endl;
	std::cout << "size until:" << decltype(s)::get_size_until<2>() << std::endl;
	std::cout << "size until:" << decltype(s)::get_size_until<0>() << std::endl;
	s.set<1>(0, 45);
	s.set<0>(1, 89);
	s.set<3>(0, 448945);
	s.set<0>(2, 1);

	s.save("file1");

	TrivialArrayOfStructs<int, double, unsigned char, unsigned long> t{
		s.get_num_rows()};
	t.read("file1");

	std::cout << "reading: " << t.get<1>(0) << "\n";
	std::cout << "reading: " << t.get<0>(1) << "\n";
	std::cout << "reading: " << t.get<3>(0) << "\n";
	std::cout << "reading: " << t.get<0>(2) << "\n";
}
