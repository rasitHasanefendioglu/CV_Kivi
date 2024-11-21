#ifndef ASSIGNMENT_PLY_UTILS_HPP
#define ASSIGNMENT_PLY_UTILS_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>

std::vector<std::vector<std::vector<float>>> readPLYFileWithNormals(const std::string& filename, int width, int height) {
    std::ifstream inFile(filename);
    std::vector<std::vector<std::vector<float>>> organized_point_cloud(width, std::vector<std::vector<float>>(height, std::vector<float>(6, std::numeric_limits<float>::quiet_NaN())));  // Initialize with NaN values

    std::string line;

    if (!inFile) {
        std::cerr << "Unable to open file\n";
        return organized_point_cloud;  // return empty opc
    }

    // Read and skip header lines until we reach end_header
    while (std::getline(inFile, line)) {
        if (line == "end_header") {
            break;
        }
    }

    int nan = 0;
    int no_nan = 0;
    int current_row = 0, current_col = 0;

    // Read data lines
    while (std::getline(inFile, line) && current_row < width) {
        std::istringstream lineStream(line);
        float x, y, z, nx, ny, nz;

        // If parsing succeeds and none of the values are NaN
        if ((lineStream >> x >> y >> z >> nx >> ny >> nz)) {
            organized_point_cloud[current_row][current_col] = { x, y, z, nx, ny, nz };
            no_nan++;
        }
        else {
            nan++;
        }

        current_col++;
        if (current_col >= height) {
            current_col = 0;
            current_row++;
        }
    }

    std::cout << "Nan: " << nan << " Not Nan: " << no_nan << std::endl;
    return organized_point_cloud;
}

void writePLYFile(const std::string& filename, const std::vector<std::vector<std::vector<float>>>& organized_point_cloud, bool organized = false) {
    std::ofstream plyFile(filename);

    if (!plyFile) {
        std::cout << "Unable to open file";
        return;
    }

    // Calculate total vertices
    size_t totalVertices = 0;
    if (!organized){
        for (const auto& row : organized_point_cloud) {
            for (const auto& pt : row) {
                if (!std::isnan(pt[0])) { // assuming if x is NaN, the entire point is invalid
                    totalVertices++;
                }
            }
        }
    } 
    else {
        totalVertices = organized_point_cloud.size() * organized_point_cloud[0].size();
    }
   

    plyFile << "ply\n";
    plyFile << "format ascii 1.0\n";
    plyFile << "element vertex " << totalVertices << "\n";
    plyFile << "property float x\n";
    plyFile << "property float y\n";
    plyFile << "property float z\n";
    plyFile << "property float nx\n";
    plyFile << "property float ny\n";
    plyFile << "property float nz\n";
    plyFile << "element face " << 0 << "\n";
    plyFile << "property list uchar int vertex_indices\n";
    plyFile << "end_header\n";

    for (const auto& row : organized_point_cloud) {
        for (const auto& pt : row) {
            if (organized || !std::isnan(pt[0])) { // assuming if x is NaN, the entire point is invalid
                plyFile << pt[0] << " " << pt[1] << " " << pt[2] << " "
                    << pt[3] << " " << pt[4] << " " << pt[5] << "\n";
            }
        }
    }
}

#endif