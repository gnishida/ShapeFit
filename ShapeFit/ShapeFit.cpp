#include "ShapeFit.h"
#include <time.h>

ShapeFit::ShapeFit() {
}

std::vector<float> ShapeFit::fit(cv::Mat& image, float threshold) {
	time_t start = clock();

	// cretae a binary image
	cv::Mat image2;
	cv::threshold(image, image2, 254, 255, CV_THRESH_BINARY);

	// compute a distance map
	cv::Mat distMap;
	cv::distanceTransform(image2, distMap, CV_DIST_L2, 3);
	distMap.convertTo(distMap, CV_32F);

	//cv::imwrite("distMap.png", distMap);
	
	int WIDTH = image.cols;
	int HEIGHT = image.rows;

	// initialize the parameters
	std::vector<float> params(4);
	params[0] = WIDTH * 0.25;
	params[1] = HEIGHT * 0.25;
	params[2] = WIDTH * 0.5;
	params[3] = HEIGHT * 0.5;

	float delta = 10.0f;
	int stepsize = 10;

	int step_count = 0;
	float dist = std::numeric_limits<float>::max();
	for (int i = 0; i < 100; ++i) {
		///////////////////////////////// DEBUG ///////////////////////////////// 
		/*
		cv::Mat hoge(HEIGHT, WIDTH, CV_8UC1, cv::Scalar(255));
		cv::rectangle(hoge, cv::Rect(params[0], params[1], params[2], params[3]), cv::Scalar(0), 1, CV_AA);
		char filename[256];
		sprintf(filename, "results/image_%04d.png", i);
		cv::imwrite(filename, hoge);
		*/
		///////////////////////////////// DEBUG ///////////////////////////////// 


		int r = i % params.size();
		float old_value = params[r];

		// dist from the proposal 1
		params[r] = std::max(0.0f, old_value - delta);
		float dist1 = distanceTransform(distMap, params);

		// dist from the proposal 2
		params[r] = old_value + delta;
		float dist2 = distanceTransform(distMap, params);

		if (dist1 <= dist2 && dist1 <= dist) {
			params[r] = std::max(0.0f, old_value - delta);
			dist = dist1;
		}
		else if (dist2 <= dist1 && dist2 <= dist) {
			params[r] = old_value + delta;
			dist = dist2;
		}
		else {
			params[r] = old_value;
		}

		if (step_count >= stepsize) {
			step_count = 0;
			delta = std::max(1.0f, delta * 0.8f);
		}

		if (dist < threshold) break;
	}

	time_t end = clock();
	std::cout << "Duration: " << (double)(end - start) / CLOCKS_PER_SEC << "sec." << std::endl;

	return params;
}

float ShapeFit::distanceTransform(const cv::Mat& distMap, const std::vector<float>& params) {
	// create a reference image
	cv::Mat ref_image(distMap.size(), CV_8UC1, cv::Scalar(1));
	cv::rectangle(ref_image, cv::Rect(params[0], params[1], params[2], params[3]), cv::Scalar(0), 1, 4);

	// compute a distance map
	cv::Mat ref_distMap;
	cv::distanceTransform(ref_image, ref_distMap, CV_DIST_L2, 3);
	ref_distMap.convertTo(ref_distMap, CV_32F);

	//cv::imwrite("ref_distMap.png", ref_distMap);
	
	// compute the squared difference
	cv::Mat tmp;
	cv::reduce((distMap - ref_distMap).mul(distMap - ref_distMap), tmp, 0, CV_REDUCE_SUM);
	cv::reduce(tmp, tmp, 1, CV_REDUCE_SUM);

	return tmp.at<float>(0, 0);
}