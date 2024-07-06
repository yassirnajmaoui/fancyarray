#include "tuple_of_vectors.hpp"
#include "trivial_array_of_structs.hpp"
#include "vector_of_tuples.hpp"

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
	tuple_of_vectors<int, double, float> sa;
	sa.get<0>() = {15, 24, 58, 69};
	sa.get<1>() = {3.14159, 2.5, 9.5, 2.999};
	sa.get<2>() = {99.14159, 202.5, 0.1, 0.859};
	sa.save("file1");

	tuple_of_vectors<int, double, float> sb;
	sb.read("file1");
	printContents(sb.get<0>());
	printContents(sb.get<1>());
	printContents(sb.get<2>());
	std::cout << std::endl;

	vector_of_tuples<int, double, float> va;
	va.get().resize(5);
	for (size_t i = 0; i < 5; i++)
	{
		va.set<0>(i, i);
		va.set<1>(i, i / 3.2);
		va.set<2>(i, i / 4.5f);
	}
	va.save("file2");

	vector_of_tuples<int, double, float> vb;
	vb.read("file2");
	std::cout << std::endl;

	trivial_array_of_structs<int, double, unsigned char, unsigned long> s{3};
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

	trivial_array_of_structs<int, double, unsigned char, unsigned long> t{
		s.get_num_rows()};
	t.read("file1");

	std::cout << "reading: " << t.get<1>(0) << "\n";
	std::cout << "reading: " << t.get<0>(1) << "\n";
	std::cout << "reading: " << t.get<3>(0) << "\n";
	std::cout << "reading: " << t.get<0>(2) << "\n";

	tuple_of_vectors<float, unsigned int, unsigned int> lm;
	lm.read_transpose("/mnt/F49A51EF9A51AF3C/_CURR/EEE/GCRecon/data/test_MOUSE_recon/acq_001_point_source_static.lmDat");
	const size_t num_events = lm.get_num_elems<0>();
	for(size_t i=0; i<35;i++)
	{
		std::cout << "timestamp: " << lm.get<0>(i) << std::endl;
		std::cout << "d1: " << lm.get<1>(i) << std::endl;
		std::cout << "d2: " << lm.get<2>(i) << std::endl;
	}
	lm.save_transpose("/mnt/F49A51EF9A51AF3C/_CURR/EEE/GCRecon/data/test_MOUSE_recon/tmp.lmDat");

}
