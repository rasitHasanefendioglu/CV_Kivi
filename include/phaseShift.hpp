#ifndef ASSIGNMENT_DTSPS_HPP
#define ASSIGNMENT_DTSPS_HPP

#include <vector>
#include "opencv2/opencv.hpp"

class DTSPS {
public:
    struct PhaseMap {
        cv::Mat phase_map_first;
        cv::Mat phase_map_second;
        cv::Mat averaged_result;
        cv::Mat unwrapped_result_first;
        cv::Mat unwrapped_result_second;
        // Adding normalized results to the struct
        cv::Mat normalized_wrapped1;
        cv::Mat normalized_wrapped2;
        cv::Mat normalized_average;
        cv::Mat normalized_unwrapped1;
        cv::Mat normalized_unwrapped2;
    };

    PhaseMap implementing();

private:
    cv::Mat createVerticalImage(const std::vector<unsigned int>& values);
    std::vector<unsigned int> generateQuantizedSinusoid(const int size, const double phase = 0.0);
    std::vector<cv::Mat> patternImages();
    cv::Mat calculatePhase(const cv::Mat& I1, const cv::Mat& I2, const cv::Mat& I3);
    cv::Mat calculateUnwrapped(const cv::Mat& wrapped_phase);
    cv::Mat phaseAverage(const cv::Mat& phase1, const cv::Mat& phase2);
    cv::Mat normalizePhaseMap(const cv::Mat& phase_map);
};

#endif // DTSPS_HPP