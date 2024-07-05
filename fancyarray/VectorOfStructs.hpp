#pragma once

#include <vector>

#include "FileCommon.hpp"
#include "TupleCommon.hpp"

template <typename... Types>
class VectorOfStructs
{
public:
	auto& get() { return tuples; }

	template <size_t row>
	constexpr auto& get(size_t i)
	{
		return std::get<row>(tuples[i]);
	}

	template <size_t row>
	constexpr auto get_sizeof() const
	{
		return sizeof(std::tuple_element<row, UsedTuple>);
	}

	constexpr auto get_sizeof_struct()
	{
		size_t struct_size = 0ull;
		if (tuples.empty())
		{
			tuples.push_back({});
		}
		call_elems_in_tuple([&struct_size]<typename T>(T&)
							{ struct_size += sizeof(T); }, tuples.at(0));
		return struct_size;
	}

	template <size_t row, typename T>
	constexpr void set(size_t i, T value)
	{
		std::get<row>(tuples[i]) = value;
	}

	constexpr size_t struct_num_elems() const
	{
		return std::tuple_size_v<UsedTuple>;
	}

	void save(const std::string& fname)
	{
		std::ofstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::out);
		checkFile(file);
		for (auto& tuple : tuples)
		{
			call_elems_in_tuple(
				[&file]<typename T>(T& elem) {
					file.write(reinterpret_cast<const char*>(&elem), sizeof(T));
				},
				tuple);
		}
	}

	void read(const std::string& fname)
	{
		std::ifstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::in);
		checkFile(file);
		// This makes the assumption that all the vectors have the same size
		const size_t struct_size = get_sizeof_struct();
		const auto begin = file.tellg();
		file.seekg(0, std::ios::end);
		const auto end = file.tellg();
		const size_t file_size = end - begin;
		if (file_size <= 0)
		{
			// File is empty
			return;
		}
		if (file_size % struct_size != 0)
		{
			throw std::runtime_error("The file is badly defined");
		}
		const size_t num_elems = file_size / struct_size;
		auto* memblock = new char[file_size];
		file.seekg(0, std::ios::beg);
		file.read(memblock, file_size);
		file.close();

		char* memblock_ptr = memblock;

		tuples.resize(num_elems);

		for (size_t i = 0; i < num_elems; i++)
		{
			call_elems_in_tuple(
				[&memblock_ptr]<typename T>(T& v)
				{
					const size_t readlength = sizeof(T);
					std::memcpy(reinterpret_cast<char*>(&v), memblock_ptr,
								readlength);
					memblock_ptr += readlength;
				},
				tuples.at(i));
		}

		delete[] memblock;
	}

private:
	using UsedTuple = std::tuple<Types...>;
	std::vector<UsedTuple> tuples;
};
