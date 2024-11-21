#include <stdexcept>
#include <cmath>

#include "phaseShift.hpp"
#include "constants.hpp"

cv::Mat DTSPS::createVerticalImage(const std::vector<unsigned int>& values) {
    cv::Mat image(assignment::image_y, assignment::image_x, CV_8UC1, cv::Scalar(0));
    int scale_factor = assignment::image_x / values.size();

    if(scale_factor * values.size() != assignment::image_x) {
        std::cerr << " Given values are not scalable to image size.";
        throw std::runtime_error("Invalid scaling factor");
    }

    int quantization_counter = 0;
    for(int i = 0; i < assignment::image_x; i++) {
        cv::line(image, cv::Point(i, 0), cv::Point(i, assignment::image_y-1), cv::Scalar(values[quantization_counter]), 1);
        if((i + 1) % scale_factor == 0) {
            quantization_counter++;
        }
    }
    return image;
}

std::vector<unsigned int> DTSPS::generateQuantizedSinusoid(const int size, const double phase) {
    std::vector<unsigned int> values;
    values.reserve(size);

    for(int i = 0; i < size; i++) {
        double x = (i * 2 * M_PI) / size;
        values.push_back(std::round((1 + sin(x + phase)) / 2 * 255));
    }
    return values;
}

std::vector<cv::Mat> DTSPS::patternImages() {
    const int pattern_number = static_cast<int>(M_PI / assignment::phase_shift);
    std::vector<cv::Mat> patterns;
    patterns.reserve(pattern_number);
    
    for(int i = 0; i < pattern_number; i++) {
        std::vector<unsigned int> values = generateQuantizedSinusoid(assignment::image_x/10, i * assignment::phase_shift);
        cv::Mat image = createVerticalImage(values);
        patterns.push_back(image.clone());
    }
    return patterns;
}

cv::Mat DTSPS::calculatePhase(const cv::Mat& I1, const cv::Mat& I2, const cv::Mat& I3) {
    CV_Assert(!I1.empty() && !I2.empty() && !I3.empty());
    CV_Assert(I1.size() == I2.size() && I2.size() == I3.size());

    cv::Mat i1, i2, i3;
    I1.convertTo(i1, CV_32F);
    I2.convertTo(i2, CV_32F);
    I3.convertTo(i3, CV_32F);

    cv::Mat numerator;
    cv::subtract(i2, i3, numerator);
    numerator *= sqrt(3.0);

    cv::Mat denominator;
    cv::addWeighted(i1, 2.0, i2, -1.0, 0.0, denominator);
    cv::subtract(denominator, i3, denominator);

    cv::Mat phase;
    cv::phase(numerator, denominator, phase);

    return phase;
}

cv::Mat DTSPS::calculateUnwrapped(const cv::Mat& wrapped_phase) {
    CV_Assert(wrapped_phase.type() == CV_32F);

    cv::Mat unwrapped_phase = cv::Mat::zeros(wrapped_phase.size(), CV_32F);
    unwrapped_phase.at<float>(0, 0) = wrapped_phase.at<float>(0, 0);
    
    for(int x = 1; x < wrapped_phase.cols; x++) {
        float diff = wrapped_phase.at<float>(0, x) - wrapped_phase.at<float>(0, x-1);
        float corrected_diff = diff;
        
        if(diff > M_PI) corrected_diff -= 2.0f * M_PI;
        else if(diff < -M_PI) corrected_diff += 2.0f * M_PI;
        
        unwrapped_phase.at<float>(0, x) = unwrapped_phase.at<float>(0, x-1) + corrected_diff;
    }
    
    for(int y = 1; y < wrapped_phase.rows; y++) {
        for(int x = 0; x < wrapped_phase.cols; x++) {
            float diff = wrapped_phase.at<float>(y, x) - wrapped_phase.at<float>(y-1, x);
            float corrected_diff = diff;
            
            if(diff > M_PI) corrected_diff -= 2.0f * M_PI;
            else if(diff < -M_PI) corrected_diff += 2.0f * M_PI;
            
            unwrapped_phase.at<float>(y, x) = unwrapped_phase.at<float>(y-1, x) + corrected_diff;
        }
    }
    
    return unwrapped_phase;
}

cv::Mat DTSPS::phaseAverage(const cv::Mat& phase1, const cv::Mat& phase2) {
    CV_Assert(phase1.type() == CV_32F && phase2.type() == CV_32F);
    CV_Assert(phase1.size() == phase2.size());

    cv::Mat diff = phase2 - phase1;
    
    for(int i = 0; i < diff.rows; i++) {
        float* row = diff.ptr<float>(i);
        for(int j = 0; j < diff.cols; j++) {
            float value = row[j];
            value = value - 2.0f * M_PI * floor((value + M_PI) / (2.0f * M_PI));
            row[j] = value;
        }
    }
    
    return phase1 + diff * 0.5f;
}

cv::Mat DTSPS::normalizePhaseMap(const cv::Mat& phase_map) {
    cv::Mat normalized;
    cv::normalize(phase_map, normalized, 0, 255, cv::NORM_MINMAX);
    normalized.convertTo(normalized, CV_8U);
    return normalized;
}

DTSPS::PhaseMap DTSPS::implementing() {
    PhaseMap toReturn;
    std::vector<cv::Mat> images = patternImages();
    
    CV_Assert(images.size() == 6);
    
    // Calculate phase maps and results
    toReturn.phase_map_first = calculatePhase(images[0], images[2], images[4]);
    toReturn.phase_map_second = calculatePhase(images[1], images[3], images[5]);
    toReturn.averaged_result = phaseAverage(toReturn.phase_map_first, toReturn.phase_map_second);
    toReturn.unwrapped_result_first = calculateUnwrapped(toReturn.phase_map_first);
    toReturn.unwrapped_result_second = calculateUnwrapped(toReturn.phase_map_second);
    
    // Normalize all results
    toReturn.normalized_wrapped1 = normalizePhaseMap(toReturn.phase_map_first);
    toReturn.normalized_wrapped2 = normalizePhaseMap(toReturn.phase_map_second);
    toReturn.normalized_average = normalizePhaseMap(toReturn.averaged_result);
    toReturn.normalized_unwrapped1 = normalizePhaseMap(toReturn.unwrapped_result_first);
    toReturn.normalized_unwrapped2 = normalizePhaseMap(toReturn.unwrapped_result_second);
    
    return toReturn;
}