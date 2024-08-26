#include "trivial_array_of_structs.hpp"
#include "trivial_struct_of_arrays.hpp"
#include "tuple_of_vectors.hpp"
#include "vector_of_tuples.hpp"

#include <iostream>

void assertion_failed(char const* expr, const long line)
{
	std::cerr << "Assertion failed: " << expr << "\n\tin line " << line
		<< std::endl;
}

/* clang-format off */
#define CHECK_LIKELY(x) __builtin_expect(x, 1)
#define ASSERT(expr) (CHECK_LIKELY(!!(expr))? ((void)0): assertion_failed(#expr, __LINE__))
/* clang-format on */

int main(int, char**)
{
	std::cout << "Testing Tuple of Vectors..." << std::endl;
	tuple_of_vectors<int, unsigned long long, char> tva;
	tva.resize(4);
	tva.set<0>(0, 15);
	tva.set<0>(1, 24);
	tva.set<0>(2, 58);
	tva.set<0>(3, 69);
	tva.set<1>(0, 4895);
	tva.set<1>(1, 45616548);
	tva.set<1>(2, 0);
	tva.set<1>(3, 256);
	tva.set<2>(0, 0);
	tva.set<2>(1, 25);
	tva.set<2>(2, 127);
	tva.set<2>(3, 96);
	tva.save("file");

	tuple_of_vectors<int, unsigned long long, char> tvb;
	tvb.read("file");
	ASSERT(tvb.get<0>(0) == 15);
	ASSERT(tvb.get<0>(1) == 24);
	ASSERT(tvb.get<0>(2) == 58);
	ASSERT(tvb.get<0>(3) == 69);
	ASSERT(tvb.get<1>(0) == 4895);
	ASSERT(tvb.get<1>(1) == 45616548);
	ASSERT(tvb.get<1>(2) == 0);
	ASSERT(tvb.get<1>(3) == 256);
	ASSERT(tvb.get<2>(0) == 0);
	ASSERT(tvb.get<2>(1) == 25);
	ASSERT(tvb.get<2>(2) == 127);
	ASSERT(tvb.get<2>(3) == 96);


	std::cout << "Testing Vector of Tuple..." << std::endl;
	vector_of_tuples<unsigned int, char, short, long> vta;
	vta.resize(4);
	vta.set<0>(0, -4815);
	vta.set<0>(1, 9999999);
	vta.set<0>(2, 0);
	vta.set<0>(3, -4895);
	vta.set<1>(0, 100);
	vta.set<1>(1, 96);
	vta.set<1>(2, 0);
	vta.set<1>(3, 127);
	vta.set<2>(0, 32767);
	vta.set<2>(1, -32767);
	vta.set<2>(2, 0);
	vta.set<2>(3, -9999);
	vta.set<3>(0, -48544);
	vta.set<3>(1, 786546);
	vta.set<3>(2, 456);
	vta.set<3>(3, -14894);
	vta.save("file");

	vector_of_tuples<unsigned int, char, short, long> vtb;
	vtb.read("file");
	ASSERT(vtb.get<0>(0) == -4815);
	ASSERT(vtb.get<0>(1) == 9999999);
	ASSERT(vtb.get<0>(2) == 0);
	ASSERT(vtb.get<0>(3) == -4895);
	ASSERT(vtb.get<1>(0) == 100);
	ASSERT(vtb.get<1>(1) == 96);
	ASSERT(vtb.get<1>(2) == 0);
	ASSERT(vtb.get<1>(3) == 127);
	ASSERT(vtb.get<2>(0) == 32767);
	ASSERT(vtb.get<2>(1) == -32767);
	ASSERT(vtb.get<2>(2) == 0);
	ASSERT(vtb.get<2>(3) == -9999);
	ASSERT(vtb.get<3>(0) == -48544);
	ASSERT(vtb.get<3>(1) == 786546);
	ASSERT(vtb.get<3>(2) == 456);
	ASSERT(vtb.get<3>(3) == -14894);


	std::cout << "Testing Trivial Array of Structures..." << std::endl;
	trivial_array_of_structs<int, double, unsigned char, unsigned long> sa{3};
	ASSERT(decltype(sa)::get_sizeof<1>() == 8);
	ASSERT(decltype(sa)::get_size_per_row() == (4+8+1+8));
	ASSERT(decltype(sa)::get_size_until<2>() == (4+8));
	ASSERT(decltype(sa)::get_size_until<0>() == 0);
	sa.set<0>(0, 89);
	sa.set<0>(1, -1800);
	sa.set<0>(2, 0);
	sa.set<2>(0, 0);
	sa.set<2>(1, 255);
	sa.set<2>(2, 128);
	sa.set<3>(0, 65536);
	sa.set<3>(1, 0);
	sa.set<3>(2, 448945);
	sa.save("file");

	trivial_array_of_structs<int, double, unsigned char, unsigned long> sb{
		sa.get_num_rows()};
	sb.read("file");
	ASSERT(sb.get<0>(0) == 89);
	ASSERT(sb.get<0>(1) == -1800);
	ASSERT(sb.get<0>(2) == 0);
	ASSERT(sb.get<2>(0) == 0);
	ASSERT(sb.get<2>(1) == 255);
	ASSERT(sb.get<2>(2) == 128);
	ASSERT(sb.get<3>(0) == 65536);
	ASSERT(sb.get<3>(1) == 0);
	ASSERT(sb.get<3>(2) == 448945);


	std::cout << "Testing Tuple of Vectors's transpose reading..." << std::endl;
	tuple_of_vectors<unsigned long long, short, unsigned int, char> tvc;
	tvc.resize(128);
	const size_t num_events = tvc.get_num_elems<0>();
	for (size_t i = 0; i < num_events; i++)
	{
		tvc.set<0>(i, i);
		tvc.set<1>(i, i * (i >> 1));
		tvc.set<2>(i, i * i);
		tvc.set<3>(i, i % 128);
	}
	tvc.save_transpose("file2");

	tuple_of_vectors<unsigned long long, short, unsigned int, char> tvd;
	tvd.read_transpose("file2");
	for (size_t i = 0; i < num_events; i++)
	{
		ASSERT(tvd.get<0>(i) == i);
		ASSERT(tvd.get<1>(i) == i*(i>>1));
		ASSERT(tvd.get<2>(i) == i*i);
		ASSERT(tvd.get<3>(i) == i % 128);
	}


	std::cout << "Testing Trivial Structure of Arrays..." << std::endl;
	trivial_struct_of_arrays<int, double, unsigned char, unsigned long> aa{3};
	ASSERT(decltype(aa)::get_sizeof<1>() == 8);
	ASSERT(decltype(aa)::get_size_per_col() == (4+8+1+8));
	ASSERT(decltype(aa)::get_size_until<2>() == (4+8));
	ASSERT(decltype(aa)::get_size_until<0>() == 0);
	aa.set<0>(0, 89);
	aa.set<0>(1, -1800);
	aa.set<0>(2, 0);
	aa.set<1>(0, -0.1);
	aa.set<1>(1, 5.2);
	aa.set<1>(2, 8.4);
	aa.set<2>(0, 0);
	aa.set<2>(1, 255);
	aa.set<2>(2, 128);
	aa.set<3>(0, 65536);
	aa.set<3>(1, 0);
	aa.set<3>(2, 448945);
	aa.save_transpose("file");

	trivial_struct_of_arrays<int, double, unsigned char, unsigned long> ab{
		sa.get_num_rows()};
	ab.read_transpose("file");
	ASSERT(ab.get<0>(0) == 89);
	ASSERT(ab.get<0>(1) == -1800);
	ASSERT(ab.get<0>(2) == 0);
	ASSERT(ab.get<1>(0) == aa.get<1>(0));
	ASSERT(ab.get<1>(1) == aa.get<1>(1));
	ASSERT(ab.get<1>(2) == aa.get<1>(2));
	ASSERT(ab.get<2>(0) == 0);
	ASSERT(ab.get<2>(1) == 255);
	ASSERT(ab.get<2>(2) == 128);
	ASSERT(ab.get<3>(0) == 65536);
	ASSERT(ab.get<3>(1) == 0);
	ASSERT(ab.get<3>(2) == 448945);

	ASSERT(ab.get_pointer<3>()[2] == aa.get<3>(2));


	// Delete used files
	std::remove("file");
	std::remove("file2");
}
