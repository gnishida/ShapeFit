﻿#include "CGA.h"
#include "GLUtils.h"
#include "OBJLoader.h"
#include <map>
#include <iostream>
#include <random>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace cga {

CGA::CGA() {
}

/**
 * Randomly select parameter values if the range is specified for the parameter
 */
std::vector<float> CGA::randomParamValues(const Grammar& grammar) {
	std::vector<float> param_values;

	for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it) {
		if (it->second.hasRange) {
			float r = (float)rand() / RAND_MAX;
			//float v = r * (it->second.range_end - it->second.range_start) + it->second.range_start;
			param_values.push_back(r);
		}
	}

	return param_values;
}

std::vector<std::pair<float, float> > CGA::getParamRanges(const Grammar& grammar) {
	std::vector<std::pair<float, float> > ranges;

	for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it) {
		if (it->second.hasRange) {
			ranges.push_back(std::make_pair(it->second.range_start, it->second.range_end));
		}
	}

	return ranges;

}

void CGA::setParamValues(Grammar& grammar, const std::vector<float>& params) {
	int count = 0;
	for (auto it = grammar.attrs.begin(); it != grammar.attrs.end(); ++it, ++count) {
		if (it->second.hasRange) {
			//grammar.attrs[it->first].value = (it->second.range_end - it->second.range_start) * params[count] + it->second.range_start;
			grammar.attrs[it->first].value = std::to_string(params[count]);
		}
	}
}

/**
 * Execute a derivation of the grammar
 */
void CGA::derive(const Grammar& grammar, bool suppressWarning) {
	shapes.clear();

	while (!stack.empty()) {
		boost::shared_ptr<Shape> shape = stack.front();
		stack.pop_front();

		if (grammar.contain(shape->_name)) {
			grammar.getRule(shape->_name).apply(shape, grammar, stack);
		} else {
			if (!suppressWarning && shape->_name.back() != '!' && shape->_name.back() != '.') {
				std::cout << "Warning: " << "no rule is found for " << shape->_name << "." << std::endl;
			}
			shapes.push_back(shape);
		}
	}
}

/**
 * Generate a geometry and add it to the render manager.
 */
void CGA::generateGeometry(RenderManager* renderManager) {
	for (int i = 0; i < shapes.size(); ++i) {
		shapes[i]->generateGeometry(renderManager, 1.0f);
	}
}

}
