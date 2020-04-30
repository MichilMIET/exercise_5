#include <vector>
#include <thread>
#include <exception>
#include <functional>
#include <iostream>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>

using namespace std;
using namespace chrono;

#define ROWS 2
#define COLS 3

#define G_ROWS 0
#define G_COLS 1


void InitMatrix(vector<vector<double>>& matrix) {
	for (size_t i = 0; i < matrix.size(); ++i) {
		for (size_t j = 0; j < matrix[0].size(); ++j) {
			matrix[i][j] = rand() % 5 + 1;
		}
	}
}


void PrintMatrix(vector<vector<double>> matrix) {
	printf("Generated matrix:\n");
	for (size_t i = 0; i < matrix.size(); ++i) {
		for (size_t j = 0; j < matrix[0].size(); ++j) {
			printf("%lf ", matrix[i][j]);
		}
		printf("\n");
	}
}


void FindAverageValues(int mode, vector<vector<double>> matrix, vector<double>& average_vals) {
	switch (mode) {
	case G_ROWS: {
		for (size_t i = 0; i < matrix.size(); ++i) {
			cilk::reducer_opadd<double> sum(0.0);
			cilk_for(size_t j = 0; j < matrix[0].size(); ++j) {
				sum += matrix[i][j];
			}
			average_vals[i] = sum.get_value() / matrix[0].size();
		}
		break;
	}
	case G_COLS: {
		for (size_t j = 0; j < matrix[0].size(); ++j) {
			cilk::reducer_opadd<double> sum(0.0);
			cilk_for(size_t i = 0; i < matrix.size(); ++i) {
				sum += matrix[i][j];
			}
			average_vals[j] = sum.get_value() / matrix.size();
		}
		break;
	}
	default: {
		throw("Incorrect value for parameter 'mode' in function FindAverageValues() call!");
	}
	}
}


void PrintAverageVals(int mode, vector<double> average_vals) {
	switch (mode) {
	case G_ROWS: {
		printf("\nAverage values in rows:\n");
		for (size_t i = 0; i < average_vals.size(); ++i) {
			printf("Row %u: %lf\n", i, average_vals[i]);
		}
		break;
	}
	case G_COLS: {
		printf("\nAverage values in columns:\n");
		for (size_t i = 0; i < average_vals.size(); ++i) {
			printf("Column %u: %lf\n", i, average_vals[i]);
		}
		break;
	}
	default: {
		throw("Incorrect value for parameter 'mode' in function PrintAverageVals() call!");
	}
	}
}


int main() {
	srand((unsigned)time(0));

	vector<vector<double>> matrix(ROWS, vector<double>(COLS));

	vector<double> average_vals_in_rows(ROWS);
	vector<double> average_vals_in_cols(COLS);
	try {
		InitMatrix(matrix);
		PrintMatrix(matrix);

		thread first_thr(FindAverageValues, G_ROWS, matrix, ref(average_vals_in_rows));
		thread second_thr(FindAverageValues, G_COLS, matrix, ref(average_vals_in_cols));

		first_thr.join();
		second_thr.join();

		PrintAverageVals(G_ROWS, average_vals_in_rows);
		PrintAverageVals(G_COLS, average_vals_in_cols);
	}
	catch (exception& except) {
		cerr << "Error: " << except.what() << endl;
		return EXIT_FAILURE;
	}
	system("pause");
}