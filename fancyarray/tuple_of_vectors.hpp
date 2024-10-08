#pragma once

#include "file_common.hpp"
#include "tuple_common.hpp"
#include "variadic_common.hpp"

#include <cstring>
#include <vector>

template <typename... Types>
class tuple_of_vectors
{
public:
	// Return the vector at the row specified by template
	template <size_t row>
	constexpr const auto& get() const
	{
		static_assert(row < tuple_size());
		return std::get<row>(vectors);
	}

	// Return the ith element of the vector at the row specified by template
	template <size_t row>
	constexpr const auto& get(size_t i) const
	{
		static_assert(row < tuple_size());
		return std::get<row>(vectors)[i];
	}

	// Return the number of elements in the vector at the specified row
	template <size_t row = 0>
	constexpr size_t get_num_elems() const
	{
		static_assert(row < tuple_size());
		return get<row>().size();
	}

	// Get the sizeof of the element at the specified row
	template <size_t row>
	static constexpr auto get_sizeof()
	{
		static_assert(row < tuple_size());
		return sizeof(get_nth_type_t<row, Types...>);
	}

	// Get the total sizeof of all the types.
	// In other words, the sizeof of a column
	static constexpr size_t get_sizeof_struct()
	{
		return get_total_sizeof_t<Types...>::FinalSum;
	}

	// Set the value of the ith element at the row specified by template
	template <size_t row, typename T>
	constexpr void set(size_t i, T value)
	{
		static_assert(row < tuple_size());
		std::get<row>(vectors)[i] = value;
	}

	// Get the number of types in the tuple
	static constexpr size_t tuple_size()
	{
		return std::tuple_size_v<decltype(vectors)>;
	}

	// Set the size of all the vectors at the same time
	void resize(size_t new_size)
	{
		call_elems_in_tuple([new_size]<typename T>(std::vector<T>& v)
							{ v.resize(new_size); }, vectors);
	}

	// Read a file as an array of structures
	void read_transpose(const std::string& fname)
	{
		// Here, "row" and "column" have switched definitions
		std::ifstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::in);
		check_file(file);
		// This makes the assumption that all the vectors have the same size
		constexpr size_t struct_size = get_sizeof_struct();
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
		file.seekg(0, std::ios::beg);

		// Resize vectors
		call_elems_in_tuple([num_elems]<typename T>(std::vector<T>& v)
							{ v.resize(num_elems); }, vectors);

		for (size_t row = 0; row < num_elems; row++)
		{
			call_elems_in_tuple(
				[&file, row]<typename T>(std::vector<T>& v)
				{
					constexpr size_t readlength = sizeof(T);
					file.read(reinterpret_cast<char*>(v.data() + row),
							  readlength);
				},
				vectors);
		}
		file.close();
	}

	// Save the structure as an array of structure
	void save_transpose(const std::string& fname) const
	{
		// Here, "row" and "column" have switched definitions
		std::ofstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::out);
		check_file(file);
		// This makes the assumption that all the vectors have the same size
		const size_t num_elems = get_num_elems();

		for (size_t row = 0; row < num_elems; row++)
		{
			call_elems_in_tuple(
				[&file, row]<typename T>(const std::vector<T>& v)
				{
					constexpr size_t readlength = sizeof(T);
					file.write(reinterpret_cast<const char*>(v.data() + row),
							   readlength);
				},
				vectors);
		}
		file.close();
	}

	// Read the data from a file
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
		file.close();
	}

	// Save to a file
	void save(const std::string& fname) const
	{
		std::ofstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::out);
		check_file(file);
		call_elems_in_tuple(
			[&file]<typename T>(const std::vector<T>& v) {
				file.write(reinterpret_cast<const char*>(v.data()),
						   v.size() * sizeof(T));
			},
			vectors);
		file.close();
	}

private:
	std::tuple<std::vector<Types>...> vectors;
};
