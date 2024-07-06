#pragma once

#include "file_common.hpp"
#include "tuple_common.hpp"

#include <cstring>
#include <vector>

template <typename... Types>
class tuple_of_vectors
{
public:
	template <size_t row>
	constexpr auto& get()
	{
		return std::get<row>(vectors);
	}

	template <size_t row>
	constexpr auto get(size_t i)
	{
		return std::get<row>(vectors)[i];
	}

	template <size_t row>
	constexpr auto get_sizeof() const
	{
		return sizeof(std::get<row>(vectors).value_type);
	}

	constexpr auto get_sizeof_struct()
	{
		size_t struct_size = 0ull;
		call_elems_in_tuple([&struct_size]<typename T>(std::vector<T>&)
							{ struct_size += sizeof(T); }, vectors);
		return struct_size;
	}

	template <size_t row, typename T>
	constexpr void set(size_t i, T value)
	{
		std::get<row>(vectors)[i] = value;
	}

	constexpr size_t struct_num_elems() const
	{
		return std::tuple_size_v<decltype(vectors)>;
	}

	void save(const std::string& fname)
	{
		std::ofstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::out);
		checkFile(file);
		call_elems_in_tuple(
			[&file]<typename T>(std::vector<T>& v) {
				file.write(reinterpret_cast<const char*>(v.data()),
						   v.size() * sizeof(T));
			},
			vectors);
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

		call_elems_in_tuple(
			[&memblock_ptr, num_elems]<typename T>(std::vector<T>& v)
			{
				const size_t readlength = num_elems * sizeof(T);
				v.resize(num_elems);
				std::memcpy(reinterpret_cast<char*>(v.data()), memblock_ptr,
							readlength);
				memblock_ptr += readlength;
			},
			vectors);

		delete[] memblock;
	}

private:
	std::tuple<std::vector<Types>...> vectors;
};
