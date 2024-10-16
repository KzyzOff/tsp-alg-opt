#include "CSVLogger.hpp"

#include <iostream>
#include <sstream>

#include "constants.hpp"

CSVLogger::CSVLogger(const std::string& filename, int n_columns)
	: filepath(std::string(SIMULATION_RESULTS_PATH).append(filename)),
	  file(filepath, std::ios_base::app),
	  n_columns(n_columns)
{
	row.resize(n_columns, "");
	if ( ! file.is_open() )
		std::cout << "Error while trying to open the file: "
				  << "'" << filename << "': " << file.bad() << std::endl;
}

void CSVLogger::commit_row() {
	std::ostringstream full_row {};
	for ( const auto& el : row ) {
		full_row << el << ';';
	}
	full_row << '\n';
	file << full_row.str();
	row.clear();
	row.resize(n_columns, "");
}

void CSVLogger::add(int col, const std::string& content) {
	if ( ! is_in_bounds(col) )
		return;

	row.at(col) = content;
}

void CSVLogger::add(int col, const int& content) {
	if ( ! is_in_bounds(col) )
		return;

	row.at(col) = std::to_string(content);
}
