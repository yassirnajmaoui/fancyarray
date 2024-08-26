#pragma once

#include "file_common.hpp"
#include "variadic_common.hpp"

#include <memory>

template <typename... Types>
class trivial_array_of_structs
{
public:
	static constexpr size_t get_num_columns() { return sizeof...(Types); }

	template <size_t column>
	static constexpr size_t get_sizeof()
	{
		return sizeof(get_nth_type_t<column, Types...>);
	}

	static constexpr size_t get_size_per_row()
	{
		return get_total_sizeof_t<Types...>::FinalSum;
	}

	template <size_t column>
	static constexpr size_t get_size_until()
	{
		return get_size_until_t<column, Types...>::FinalSum;
	}

	explicit trivial_array_of_structs(const size_t p_num_rows) :
		num_rows(p_num_rows)
	{
		buffer = std::make_unique<char[]>(num_rows * get_size_per_row());
	}

	size_t get_total_size() const { return num_rows * get_size_per_row(); }

	size_t get_num_rows() const { return num_rows; }

	template<size_t column>
	static size_t get_total_offset(const size_t row)
	{
		static_assert(column < get_num_columns(), "Column index out of bounds");
		constexpr size_t offset_col = get_size_until<column>();
		const size_t offset_row = get_size_per_row() * row;
		return offset_row + offset_col;
	}

	template <size_t column>
	get_nth_type_t<column, Types...> get(const size_t row) const
	{
		return *reinterpret_cast<get_nth_type_t<column, Types...>*>(
			buffer.get() + get_total_offset<column>(row));
	}

	template <size_t column>
	void set(const size_t row, get_nth_type_t<column, Types...> value)
	{
		*reinterpret_cast<get_nth_type_t<column, Types...>*>(
			buffer.get() + get_total_offset<column>(row)) = value;
	}

	void save(const std::string& fname) const
	{
		std::ofstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::out);
		check_file(file);
		file.write(buffer.get(), get_total_size());
		file.close();
	}

	void read(const std::string& fname) const
	{
		std::ifstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::in);
		check_file(file);
		file.read(buffer.get(), get_total_size());
		file.close();
	}

private:
	std::unique_ptr<char[]> buffer;
	size_t num_rows;
};
