#pragma once
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace glm;
using namespace std;

struct Triangle {
	vec3 p1, p2, p3;
};

mat4 TransformMatrix(vec3 rotateCtrl, vec3 translateCtrl, vec3 scaleCtrl) {
	glm::mat4 unit(    // 单位矩阵
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1)
	);
	mat4 scale = glm::scale(unit, scaleCtrl);
	mat4 translate = glm::translate(unit, translateCtrl);
	mat4 rotate = unit;
	rotate = glm::rotate(rotate, glm::radians(rotateCtrl.x), glm::vec3(1, 0, 0));
	rotate = glm::rotate(rotate, glm::radians(rotateCtrl.y), glm::vec3(0, 1, 0));
	rotate = glm::rotate(rotate, glm::radians(rotateCtrl.z), glm::vec3(0, 0, 1));

	mat4 model = translate * rotate * scale;
	return model;
}

void loadObj(string filepath, vector<Triangle>& triangles) {


	

	/*

	*/
	vector<vec3> vertices;
	vector<int> indices;

	ifstream fin(filepath);
	string line;
	if (!fin.is_open()) {
		cout << "failed to load obj file!" << endl;
		exit(-1);
	}

	while (std::getline(fin, line)) {
		istringstream sin(line);
		string type;
		float f1, f2, f3;
		int index_1, index_2, index_3;

		sin >> type;
		if (type == "v") {
			sin >> f1 >> f2 >> f3;
			vertices.push_back(vec3(f1, f2, f3));
		}

		if (type == "f") {
			sin >> index_1 >> index_2 >> index_3;
			indices.push_back(index_1 - 1);
			indices.push_back(index_2 - 1);
			indices.push_back(index_3 - 1);
		}
	}

	// 模型坐标变换
	for (auto& v : vertices) {
		v.x *= 5.0, v.y *= 5.0, v.z *= 5.0;
		v.y -= 0.5;
	}

	//把三角形的顶点数据存到triangles中 indices.size() / 3
	/**/
	for (int i = 0; i < 8100; i += 3) {
		Triangle t;
		t.p1 = vertices[indices[i]];
		t.p2 = vertices[indices[i + 1]];
		t.p3 = vertices[indices[i + 2]];
		/*cout << t.p1.x << " " << t.p1.y << " " << t.p1.z << endl;
		cout << t.p2.x << " " << t.p2.y << " " << t.p2.z << endl;
		cout << t.p3.x << " " << t.p3.y << " " << t.p3.z << endl;*/
		triangles.push_back(t);
	}
	cout << "Number of triangles: " << triangles.size() << endl;
}
