#pragma once

#include <fstream>

template <typename FileIOType>
void check_file(const FileIOType& file)
{
	if (!file.is_open())
	{
		throw std::system_error(
			std::make_error_code(std::errc::no_such_file_or_directory),
			"Unable to open file");
	}
}
