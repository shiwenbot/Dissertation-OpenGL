// Good academic practice, this file contains code from learnOpenGL
#include "Camera.h"

Camera::Camera(int ScreenWidth, int ScreenHeight){}

void Camera::OnMouseMoved(float xpos, float ypos) {
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	cameraFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront.y = sin(glm::radians(Pitch));
	cameraFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(cameraFront);
	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	LeftBottomCorner = cameraFront - halfW * cameraRight - halfH * cameraUp;
	LoopNum = 0;
}

void Camera::SetVariables(Shader shader) {
	// fboManager绑定的纹理被定义为纹理0，所以这里设置片段着色器中的historyTexture为纹理0
	shader.setInt("historyTexture", 0);
	// 相机参数赋值
	shader.setVec3("camera.camPos", cameraPos);
	shader.setVec3("camera.front", cameraFront);
	shader.setVec3("camera.right", cameraRight);
	shader.setVec3("camera.up", cameraUp);
	shader.setFloat("camera.halfH", halfH);
	shader.setFloat("camera.halfW", halfW);
	shader.setVec3("camera.leftbottom", LeftBottomCorner);
	shader.setInt("camera.LoopNum", LoopNum);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
	float velocity = cameraSpeed * deltaTime;
	if (direction == FORWARD) cameraPos += cameraFront * velocity;
	if (direction == BACKWARD) cameraPos -= cameraFront * velocity;
	if (direction == LEFT) cameraPos -= cameraRight * velocity;
	if (direction == RIGHT) cameraPos += cameraRight * velocity;
	if (direction == UP) cameraPos += cameraUp * velocity;
	if (direction == DOWN) cameraPos -= cameraUp * velocity;

	LeftBottomCorner = cameraFront - halfW * cameraRight - halfH * cameraUp;

	LoopNum = 0;
}