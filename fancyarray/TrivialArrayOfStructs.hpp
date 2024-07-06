#pragma once

#include "FileCommon.hpp"

#include <memory>

// ---------- Selecting a type ---------
template <size_t Index, typename... Types>
struct get_nth_type;

// Base case: end of recursion
template <typename First, typename... Rest>
struct get_nth_type<0, First, Rest...>
{
	using Type = First;
};

template <size_t Index, typename First, typename... Rest>
struct get_nth_type<Index, First, Rest...>
{
	using Type = typename get_nth_type<Index - 1, Rest...>::Type;
};

// Helper alias template
template <size_t Index, typename... Types>
using get_nth_type_t = typename get_nth_type<Index, Types...>::Type;


// ---------- Summing the sizes ---------
template <size_t Sum, typename... Types>
struct get_total_sizeof;

// Base case: end of recursion
template <size_t Sum, typename Last>
struct get_total_sizeof<Sum, Last>
{
	static constexpr size_t FinalSum = Sum + sizeof(Last);
};

template <size_t Sum, typename First, typename... Rest>
struct get_total_sizeof<Sum, First, Rest...>
{
	static constexpr size_t FinalSum =
		get_total_sizeof<Sum + sizeof(First), Rest...>::FinalSum;
};

// Helper alias
template <typename... Types>
using get_total_sizeof_t = get_total_sizeof<0, Types...>;


// ---------- Summing the sizes ---------
template <size_t Index, typename... Types>
struct get_size_until;

// Base case: end of recursion
template <typename First, typename... Types>
struct get_size_until<0, First, Types...>
{
	static constexpr size_t FinalSum = 0;
};

template <size_t Index, typename First, typename... Rest>
struct get_size_until<Index, First, Rest...>
{
	static_assert(Index <= sizeof...(Rest) + 1, "Index out of bounds");
	static constexpr size_t FinalSum =
		sizeof(First) + get_size_until<Index - 1, Rest...>::FinalSum;
};

// Helper alias
template <size_t Index, typename... Types>
using get_size_until_t = get_size_until<Index, Types...>;


// -------------------------------------
template <typename... Types>
class TrivialArrayOfStructs
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

	explicit TrivialArrayOfStructs(const size_t p_num_rows) :
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
		checkFile(file);
		file.write(buffer.get(), get_total_size());
		file.close();
	}

	void read(const std::string& fname) const
	{
		std::ifstream file;
		file.open(fname.c_str(), std::ios::binary | std::ios::in);
		checkFile(file);
		file.read(buffer.get(), get_total_size());
		file.close();
	}

private:
	std::unique_ptr<char[]> buffer;
	size_t num_rows;
};
