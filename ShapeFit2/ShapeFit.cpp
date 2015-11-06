#include "ShapeFit.h"
#include "Rectangle.h"
#include <time.h>
#include "GLWidget3D.h"

ShapeFit::ShapeFit() {
}

void ShapeFit::fit(cv::Mat& image, float threshold, float& offset_x, float& offset_y, float& object_width, float& object_depth, std::vector<float>& params2) {
	time_t start = clock();

	// cretae a binary image
	cv::Mat image2;
	cv::threshold(image, image2, 254, 255, CV_THRESH_BINARY);

	// compute a distance map
	cv::Mat distMap;
	cv::distanceTransform(image2, distMap, CV_DIST_L2, 3);
	distMap.convertTo(distMap, CV_32F);
	
	int WIDTH = image.cols;
	int HEIGHT = image.rows;

	// initialize the parameters
	std::vector<std::pair<float, float> > ranges = cga::CGA::getParamRanges(grammar);
	ranges.push_back(std::make_pair(-20, 20));	// x
	ranges.push_back(std::make_pair(-20, 20));	// y
	ranges.push_back(std::make_pair(1, 16));	// w
	ranges.push_back(std::make_pair(1, 16));	// h
	std::vector<float> params(ranges.size());
	for (int i = 0; i < params.size(); ++i) params[i] = 0.5f;

	float delta = 0.1f;
	int stepsize = 10;

	int step_count = 0;
	float dist = std::numeric_limits<float>::max();
	for (int i = 0; i < 100; ++i) {
		///////////////////////////////// DEBUG ///////////////////////////////// 
		/*
		QImage hoge = renderImage(params, ranges);
		char filename[256];
		sprintf(filename, "results/image_%04d.png", i);
		hoge.save(filename);
		*/
		///////////////////////////////// DEBUG ///////////////////////////////// 
		
		int r = i % params.size();
		float old_value = params[r];

		// dist from the proposal 1
		params[r] = std::max(0.0f, old_value - delta);
		float dist1 = distanceTransform(distMap, params, ranges);

		// dist from the proposal 2
		params[r] = std::min(1.0f, old_value + delta);
		float dist2 = distanceTransform(distMap, params, ranges);

		if (dist1 <= dist2 && dist1 <= dist) {
			params[r] = old_value - delta;
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
			delta = std::max(0.01f, delta * 0.8f);
		}

		if (dist < threshold) break;
	}

	time_t end = clock();
	std::cout << "Duration: " << (double)(end - start) / CLOCKS_PER_SEC << "sec." << std::endl;

	convertParams(params, ranges, offset_x, offset_y, object_width, object_depth, params2);
}

float ShapeFit::distanceTransform(const cv::Mat& distMap, const std::vector<float>& params, const std::vector<std::pair<float, float> >& ranges) {
	QImage ref_image = renderImage(params, ranges);

	ref_image.save("results/image.png");
	cv::Mat mat(ref_image.height(), ref_image.width(), CV_8UC4, ref_image.bits(), ref_image.bytesPerLine());
	cv::Mat grayMat;
	cv::cvtColor(mat, grayMat, CV_BGR2GRAY);

	// compute a distance map
	cv::Mat ref_distMap;
	cv::distanceTransform(grayMat, ref_distMap, CV_DIST_L2, 3);
	ref_distMap.convertTo(ref_distMap, CV_32F);

	cv::imwrite("results/ref_distMap.png", ref_distMap);
	
	// compute the squared difference
	cv::Mat tmp;
	cv::reduce((distMap - ref_distMap).mul(distMap - ref_distMap), tmp, 0, CV_REDUCE_SUM);
	cv::reduce(tmp, tmp, 1, CV_REDUCE_SUM);

	return tmp.at<float>(0, 0);
}

QImage ShapeFit::renderImage(const std::vector<float>& params, const std::vector<std::pair<float, float> >& ranges) {
	std::vector<float> params2;
	float offset_x;
	float offset_y;
	float object_width;
	float object_depth;
	convertParams(params, ranges, offset_x, offset_y, object_width, object_depth, params2);

	glWidget->renderManager.removeObjects();
	glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
	glUseProgram(glWidget->renderManager.program);

	// set camera
	glWidget->camera.xrot = 25.0f;
	glWidget->camera.yrot = -40.0f;
	glWidget->camera.zrot = 0.0f;
	glWidget->camera.pos = glm::vec3(0, 0, 40.0f);
	glWidget->camera.updateMVPMatrix();

	// lot
	cga::Rectangle* start = new cga::Rectangle("Start", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(offset_x - (float)object_width*0.5f, offset_y - (float)object_depth*0.5f, 0)), glm::mat4(), object_width, object_depth, glm::vec3(1, 1, 1));
	glWidget->system.stack.push_back(boost::shared_ptr<cga::Shape>(start));

	// generate geometry
	cga::CGA::setParamValues(grammar, params2);
	glWidget->system.derive(grammar, true);
	glWidget->system.generateGeometry(&glWidget->renderManager);

	// render
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	glUniform1i(glGetUniformLocation(glWidget->renderManager.program, "seed"), rand() % 100);

	// Model view projection行列をシェーダに渡す
	glUniformMatrix4fv(glGetUniformLocation(glWidget->renderManager.program, "mvpMatrix"), 1, GL_FALSE, &glWidget->camera.mvpMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(glWidget->renderManager.program, "mvMatrix"), 1, GL_FALSE, &glWidget->camera.mvMatrix[0][0]);

	// pass the light direction to the shader
	//glUniform1fv(glGetUniformLocation(renderManager.program, "lightDir"), 3, &light_dir[0]);
	//glUniform3f(glGetUniformLocation(renderManager->program, "lightDir"), light_dir.x, light_dir.y, light_dir.z);

	// obtain the image from the frame buffer
	glWidget->drawScene(0);
	return glWidget->grabFrameBuffer();
}

void ShapeFit::convertParams(const std::vector<float>& params, const std::vector<std::pair<float, float> >& ranges, float& offset_x, float& offset_y, float& object_width, float& object_depth, std::vector<float>& params2) {
	params2.resize(params.size() - 4);
	for (int i = 0; i < params.size() - 4; ++i) {
		params2[i] = ranges[i].first + (ranges[i].second - ranges[i].first) * params[i];
	}

	int offset = params.size() - 4;
	offset_x = ranges[offset + 0].first + (ranges[offset + 0].second - ranges[offset + 0].first) * params[offset + 0];
	offset_y = ranges[offset + 1].first + (ranges[offset + 1].second - ranges[offset + 1].first) * params[offset + 1];
	object_width = ranges[offset + 2].first + (ranges[offset + 2].second - ranges[offset + 2].first) * params[offset + 2];
	object_depth = ranges[offset + 3].first + (ranges[offset + 3].second - ranges[offset + 3].first) * params[offset + 3];
}