#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "macros.h"
#include "Shader.h"

using namespace glm;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
private:
	vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
	vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
	// camera direction
	vec3 cameraFront;
	vec3 cameraUp;
	vec3 cameraRight;

	vec3 LeftBottomCorner;
	
	float fov = 45.0f;
	float Pitch = 0.0f;
	float Yaw = -90.0f;
	const float cameraSpeed = 0.8f;
	const float sensitivity = 0.05f;

	float lastX;
	float lastY;

	float halfH = tan(fov);
	float halfW = halfH * ScreenRatio;
public:
	int LoopNum = 0;
public:
	Camera(int ScreenWidth, int ScreenHeight);
	void OnMouseMoved(float xpos, float ypos);
	void SetVariables(Shader shader);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
};

class timeRecord {
public:
	timeRecord() {}

	void updateTime() {
		currentFrameTime = static_cast<float>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
	}

private:
	float currentFrameTime = 0.0f;
	float lastFrameTime = 0.0f;
public:
	float deltaTime = 0.0f;
};