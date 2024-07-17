#include "tuple_of_vectors.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <random>

using MyToV = tuple_of_vectors<int, float, double, char>;

void timeFunc(const std::string& funcName, const std::function<void()>& f)
{
	// Start timing
	const auto start = std::chrono::high_resolution_clock::now();

	f();

	// Stop timing
	const auto end = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> elapsed = end - start;

	std::cout << "Time taken for " << funcName << ": " << elapsed.count()
			  << " seconds" << std::endl;
}

void fillVectors(MyToV& vectors, const size_t newSize)
{
	vectors.resize(newSize);

	timeFunc("filling vectors",
			 [&vectors, newSize]()
			 {
				 // Seed with a real random value, if available
				 std::random_device rd;
				 // Engine and distribution
				 std::default_random_engine eng(rd());

				 std::discrete_distribution<int> distr0(0, 1 << 30);
				 std::exponential_distribution<float> distr1(5.0);
				 std::uniform_real_distribution<double> distr2(5.0, 25.0);
				 std::poisson_distribution<char> distr3(96);

				 // Perform some operations
				 for (size_t i = 0; i < newSize; ++i)
				 {
					 vectors.set<0>(i, distr0(eng));
					 vectors.set<1>(i, distr1(eng));
					 vectors.set<2>(i, distr2(eng));
					 vectors.set<3>(i, distr3(eng));
				 }
			 });
}

int main(int, char**)
{
	constexpr size_t DataSize = 1000000; // 1 million elements
	MyToV tov;

	fillVectors(tov, DataSize);

	timeFunc("saving transposed",
			 [&tov]() { tov.save_transpose("tmp_tov_transpose.dat"); });

	timeFunc("saving", [&tov]() { tov.save("tmp_tov.dat"); });

	timeFunc("reading transposed",
			 [&tov]() { tov.read_transpose("tmp_tov_transpose.dat"); });

	timeFunc("reading", [&tov]() { tov.read("tmp_tov.dat"); });

	return 0;
}
