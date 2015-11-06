#pragma once

#include <opencv2/opencv.hpp>

class ShapeFit {
public:
	ShapeFit();

	std::vector<float> fit(cv::Mat& image, float threshold);

private:
	float distanceTransform(const cv::Mat& distMap, const std::vector<float>& params);
};

