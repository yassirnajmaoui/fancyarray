#pragma once

#include "file_common.hpp"
#include "variadic_common.hpp"

#include <memory>

template <typename... Types>
class trivial_struct_of_arrays
{
public:
    static constexpr size_t get_num_rows() { return sizeof...(Types); }

    template <size_t row>
    static constexpr size_t get_sizeof()
    {
        return sizeof(get_nth_type_t<row, Types...>);
    }

    static constexpr size_t get_size_per_col()
    {
        return get_total_sizeof_t<Types...>::FinalSum;
    }

    template <size_t row>
    static constexpr size_t get_size_until()
    {
        return get_size_until_t<row, Types...>::FinalSum;
    }

    explicit trivial_struct_of_arrays(const size_t p_num_columns) :
        num_columns(p_num_columns)
    {
        buffer = std::make_unique<char[]>(num_columns * get_size_per_col());
    }

    template <size_t row>
    size_t get_size_of_row() const
    {
        return num_columns * get_sizeof<row>();
    }

    size_t get_total_size() const { return num_columns * get_size_per_col(); }

    size_t get_num_columns() const { return num_columns; }

    template <size_t row>
    size_t get_total_offset(const size_t col) const
    {
        static_assert(row < get_num_rows(), "Row index out of bounds");
        const size_t offset_row = get_size_until<row>() * num_columns;
        const size_t offset_col = get_sizeof<row>() * col;
        return offset_row + offset_col;
    }

    template <size_t row>
    get_nth_type_t<row, Types...> get(const size_t col) const
    {
        return *reinterpret_cast<get_nth_type_t<row, Types...>*>(
            buffer.get() + get_total_offset<row>(col));
    }

    template <size_t row>
    get_nth_type_t<row, Types...>* get_pointer() const
    {
        static_assert(row < get_num_rows(), "Row index out of bounds");
        return reinterpret_cast<get_nth_type_t<row, Types...>*>(
            buffer.get() + get_size_until<row>() * num_columns);
    }

    template <size_t row>
    void set(const size_t col, get_nth_type_t<row, Types...> value)
    {
        *reinterpret_cast<get_nth_type_t<row, Types...>*>(
            buffer.get() + get_total_offset<row>(col)) = value;
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

    void read_transpose(const std::string& fname)
    {
        std::ifstream file(fname, std::ios::binary);
        check_file(file);

        constexpr size_t struct_size = get_size_per_col();
        char struct_buffer[struct_size];

        for (size_t col = 0; col < num_columns; ++col)
        {
            file.read(struct_buffer, struct_size);

            size_t offset = 0;
            transpose_helper<0>(struct_buffer, col, offset);
        }

        file.close();
    }

    void save_transpose(const std::string& fname) const
    {
        std::ofstream file(fname, std::ios::binary);
        check_file(file);

        constexpr size_t struct_size = get_size_per_col();
        char struct_buffer[struct_size];

        // Iterate over each column (each structure in AoS)
        for (size_t col = 0; col < num_columns; ++col)
        {
            size_t offset = 0;
            assemble_structure<0>(struct_buffer, col, offset);

            // Write the entire structure (one row in AoS format) to the file
            file.write(struct_buffer, struct_size);
        }

        file.close();
    }
private:
    template <size_t row>
    void transpose_helper(const char* struct_buffer, const size_t col, size_t& offset)
    {
        using T = get_nth_type_t<row, Types...>;
        const T value = *reinterpret_cast<const T*>(struct_buffer + offset);

        set<row>(col, value);
        offset += sizeof(T);

        if constexpr (row + 1 < get_num_rows())
        {
            transpose_helper<row + 1>(struct_buffer, col, offset);
        }
    }

    template <size_t row>
    void assemble_structure(char* struct_buffer, const size_t col, size_t& offset) const
    {
        using T = get_nth_type_t<row, Types...>;
        T value = get<row>(col);

        *reinterpret_cast<T*>(struct_buffer + offset) = value;
        offset += sizeof(T);

        if constexpr (row + 1 < get_num_rows())
        {
            assemble_structure<row + 1>(struct_buffer, col, offset);
        }
    }

    std::unique_ptr<char[]> buffer;
    size_t num_columns;
};
