#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

class CSVLogger {
public:
	CSVLogger() = default;
	CSVLogger(const std::filesystem::path &filename, int n_columns);

	void commit_row();
	void add(int col, const std::string& content);
	void add(int col, const int& content);
	void add(int col, const float& content);

	void set_output_file(const std::filesystem::path &filename);
	void set_col_number(int n);

private:
	std::filesystem::path filepath;
	std::ofstream file;
	int n_columns{};
	std::vector<std::string> row;

	int flush_counter;

	bool is_in_bounds(int col_index) const { return col_index < row.size(); }

};
