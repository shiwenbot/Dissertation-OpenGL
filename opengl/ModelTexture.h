#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <cmath>
#include <vector>
#include <vector>
#include "Shader.h"
#include "Mesh.h"


using namespace std;
using namespace glm;
void loadObj(string filepath, vector<vec3>& vertices, vector<int>& indices);
class ModelTexture {
public:
	GLuint id_tex_vertex;
	GLuint id_tex_index;
	unsigned int vertex_count = 0; //mesh中一共有多少个顶点
	unsigned int index_count = 0; //mesh中一共有多少个索引，可以得到三角形的个数
	vector<vec3> vertices;
	vector<int> indices;

public:
	void processData(string& filepath, Shader& shader) {
		loadObj(filepath, vertices, indices);
		vertex_count = vertices.size();
		index_count = indices.size();
		//把值打印出来，为了debug
		cout << "vertex_count: " << vertex_count << endl;
		cout << "index_count: " << index_count << endl;

		//计算存数据的纹理的宽度和高度
		int width = ceilf((float)sqrtf(vertex_count));
		int height = ceilf((float)vertex_count / (float)width);

		float* data_vertex = new float[3 * width * height];

		//遍历data，把数据存到data_vertex中
		int index = 0;
		for (int i = 0; i < vertex_count; i++) {
			data_vertex[index++] = 4 * vertices[i].x;
			data_vertex[index++] = 4 * vertices[i].y - 0.5;
			data_vertex[index++] = 4 * vertices[i].z;
		}
		cout << "size of vertex array: " << index << endl;

		//按照上面的形式处理顶点索引
		int width_index = ceilf((float)sqrtf(index_count));
		int height_index = ceilf((float)index_count / (float)width_index);
		float* data_index = new float[width_index * height_index];
		index = 0;
		//这里遍历的形式可以改成Hilbert curve
		for (int i = 0; i < index_count; i++) {
			data_index[index++] = indices[i];
		}
		cout << "size of index array: " << index << endl;

		shader.use();
		//生成纹理
		glGenTextures(1, &id_tex_vertex);
		glBindTexture(GL_TEXTURE_2D, id_tex_vertex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width * 3, height, 0, GL_RED, GL_FLOAT, data_vertex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//把纹理绑定到片段着色器中
		shader.setInt("objMainTexture", 1);

		//生成索引纹理
		glGenTextures(1, &id_tex_index);
		glBindTexture(GL_TEXTURE_2D, id_tex_index);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_index, height_index, 0, GL_RED, GL_FLOAT, data_index);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//把纹理绑定到片段着色器中
		shader.setInt("objVertIdxTexture", 2);

		delete[] data_vertex;
		delete[] data_index;
	}
	//激活并绑定纹理
	void bindTexture(Shader& shader) {
		shader.setInt("vertex_count", vertex_count);
		shader.setInt("index_count", index_count);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, id_tex_vertex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, id_tex_index);
		shader.setInt("objMainTexture", 1);
		shader.setInt("objVertIdxTexture", 2);
	}
};


