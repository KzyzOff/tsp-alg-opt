#pragma once

#include <fstream>
#include <string>
#include <vector>

class CSVLogger {
public:
	CSVLogger(const std::string& filename, int n_columns);

	void commit_row();
	void add(int col, const std::string& content);
	void add(int col, const int& content);
	void add(int col, const float& content);

private:
	std::string filepath;
	std::ofstream file;
	int n_columns;
	std::vector<std::string> row;

	bool is_in_bounds(int col_index) const { return col_index < row.size(); }

};
