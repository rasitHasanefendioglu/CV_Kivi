#ifndef ASSIGNMENT_CSV_UTILS_HPP
#define ASSIGNMENT_CSV_UTILS_HPP

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
std::vector<std::vector<double>> readCSV(std::string path){
    std::ifstream file(path);
    std::vector<std::vector<double>> matrix;
    std::string line, value;
    while(std::getline(file, line)) {
        std::vector<double> row;
        std::stringstream ss(line);
        while (std::getline(ss, value, ',')) {
            row.push_back(std::stod(value));
        }
        matrix.push_back(row);
    }
}
#endif