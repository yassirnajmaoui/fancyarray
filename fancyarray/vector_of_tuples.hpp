#pragma once

#include "file_common.hpp"
#include "tuple_common.hpp"
#include "variadic_common.hpp"

#include <vector>

template <typename... Types>
class vector_of_tuples
{
public:
	auto& get() { return tuples; }

	template <size_t column>
	constexpr auto& get(size_t i) const
	{
		static_assert(column < tuple_size());
		return std::get<column>(tuples[i]);
	}

	template <size_t column>
	constexpr auto get_sizeof() const
	{
		static_assert(column < tuple_size());
		return sizeof(std::tuple_element<column, used_tuple>);
	}

	static constexpr auto get_sizeof_struct()
	{
		return get_total_sizeof_t<Types...>::FinalSum;
	}

	template <size_t column, typename T>
	constexpr void set(size_t i, T value)
	{
		static_assert(column < tuple_size());
		std::get<column>(tuples[i]) = value;
	}

	static constexpr size_t tuple_size()
	{
		return std::tuple_size_v<used_tuple>;
	}

	void resize(size_t new_size) { tuples.resize(new_size); }

	void save(const std::string& fname) const
	{
		std::ofstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::out);
		check_file(file);
		for (auto& tuple : tuples)
		{
			call_elems_in_tuple(
				[&file]<typename T>(const T& elem) {
					file.write(reinterpret_cast<const char*>(&elem), sizeof(T));
				},
				tuple);
		}
	}

	void read(const std::string& fname)
	{
		std::ifstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::in);
		check_file(file);
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
	using used_tuple = std::tuple<Types...>;
	std::vector<used_tuple> tuples;
};
