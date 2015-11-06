#pragma once

#include <opencv2/opencv.hpp>
#include "CGA.h"

class GLWidget3D;

class ShapeFit {
public:
	cga::Grammar grammar;
	GLWidget3D* glWidget;

public:
	ShapeFit();

	void fit(cv::Mat& image, float threshold, float& offset_x, float& offset_y, float& object_width, float& object_depth, std::vector<float>& params2);
	static void convertParams(const std::vector<float>& params, const std::vector<std::pair<float, float> >& ranges, float& offset_x, float& offset_y, float& object_width, float& object_depth, std::vector<float>& params2);

private:
	float distanceTransform(const cv::Mat& distMap, const std::vector<float>& params, const std::vector<std::pair<float, float> >& ranges);
	QImage renderImage(const std::vector<float>& params, const std::vector<std::pair<float, float> >& ranges);
};

