#include <iostream>
#include "phaseShift.hpp"

int main(int argc, char* argv[]) {

    if(argc != 2 || !(argv[1] == "Q1" || argv[1] == "Q2" || argv[1] == "Q3")) {
        std::ccerr << "Usage ./KIVI_app Q[1-3]"
        return 1;
    }

    if(argv[1] == "Q1"){
        try {
            DTSPS obj;
            DTSPS::PhaseMap result = obj.implementing();

            // Save the normalized images
            cv::imwrite("wrapped1.png", result.normalized_wrapped1);
            cv::imwrite("wrapped2.png", result.normalized_wrapped2);
            cv::imwrite("average.png", result.normalized_average);
            cv::imwrite("unwrapped1.png", result.normalized_unwrapped1);
            cv::imwrite("unwrapped2.png", result.normalized_unwrapped2);

        } catch(const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    if(argv[1] == "Q2"){
        
    }
    return 0;
}