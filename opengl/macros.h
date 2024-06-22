#pragma once
const float ScreenVertices[] = {
	//position     //texture

	//triangle1
	-1.0f, 1.0f,   0.0f, 1.0f, //left top
	-1.0f, -1.0f,  0.0f, 0.0f, //left bottom
	1.0f, -1.0f,   1.0f, 0.0f, //right bottom

	//triangle2
	-1.0f,  1.0f,  0.0f, 1.0f, //left top
	1.0f, -1.0f,   1.0f, 0.0f, //right bottom
	1.0f,  1.0f,   1.0f, 1.0f  //right top
};

static unsigned int SCR_WIDTH = 1200;
static unsigned int SCR_HEIGHT = 800;
static float ScreenRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

static float GetCPURandom() {
	return (float)rand() / (RAND_MAX + 1.0);
}