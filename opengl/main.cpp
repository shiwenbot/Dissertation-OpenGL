/*
* Good academic practic: Main.cpp is modified from https://learnopengl.com/
*/
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Screen.h"
#include "ScreenFBO.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "ModelTexture.h"
#include "Triangle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void addObjToScene(Shader shader);

timeRecord tRecord;

Camera cam(SCR_WIDTH, SCR_HEIGHT);
//ModelTexture modelTexture;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Path Tracer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // don't display cursor

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// load shaders
	Shader RayTracerShader("RayTracerVertexShader.glsl", "RayTracerFragmentShader.glsl");
	Shader ScreenShader("ScreenVertexShader.glsl", "ScreenFragmentShader.glsl");
	
	// ����Ļ������λ��
	MyScreen screen;

	//��ȡģ�����ݣ��浽vector��
	string bunnyPath = "./models/bunny.obj";
	vector<Triangle> triangles;
	loadObj(bunnyPath, triangles);

	// ��������
	GLuint tbo0;
	glGenBuffers(1, &tbo0);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
	glBufferData(GL_TEXTURE_BUFFER, triangles.size() * sizeof(Triangle), &triangles[0], GL_STATIC_DRAW);
	GLuint trianglesTextureBuffer;
	glGenTextures(1, &trianglesTextureBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);

	// ������ĻFrameBuffer
	FBOManager fboManager(SCR_WIDTH, SCR_HEIGHT);

	// ��Ⱦ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		tRecord.updateTime();

		// ����
		processInput(window);

		cam.LoopNum++;

		// ����׷����Ⱦ��ǰ֡
		{
			// �󶨵���ǰ֡������
			fboManager.Bind(cam.LoopNum);

			RayTracerShader.use();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
			RayTracerShader.setInt("triangles", 1);
			RayTracerShader.setInt("triangles_count", triangles.size());

			cam.SetVariables(RayTracerShader); // pass camera attributes to shader
			addObjToScene(RayTracerShader); // set scene
			
			screen.DrawScreen();
		}

		// ��Ⱦ��Ĭ��Buffer��
		{
			// �󶨵�Ĭ�ϻ�����
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// ����
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			ScreenShader.use();
			fboManager.BindAsTexture(cam.LoopNum);
			// fboManager�󶨵���������Ϊ����0��������������Ƭ����ɫ���е�screenTextureΪ����0
			ScreenShader.setInt("screenTexture", 0);
			// ������Ļ
			screen.DrawScreen();
		}

		// ����Buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	// release resources
	fboManager.Delete();
	screen.Delete();

	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, tRecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, tRecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, tRecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, tRecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cam.ProcessKeyboard(UP, tRecord.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cam.ProcessKeyboard(DOWN, tRecord.deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	cam.OnMouseMoved(xpos, ypos);
}

void addObjToScene(Shader shader) {
	// �������ֵ��ֵ
	shader.setFloat("randOrigin", 711521.0f * (GetCPURandom()));
	// �����帳ֵ
	/*shader.setFloat("sphere[0].radius", 0.5);
	shader.setVec3("sphere[0].center", glm::vec3(0.0, 0.0, -1.0));
	shader.setFloat("sphere[1].radius", 0.5);
	shader.setVec3("sphere[1].center", glm::vec3(1.0, 0.0, -1.0));
	shader.setFloat("sphere[2].radius", 0.5);
	shader.setVec3("sphere[2].center", glm::vec3(-1.0, 0.0, -1.0));
	shader.setFloat("sphere[3].radius", 100.0);
	shader.setVec3("sphere[3].center", glm::vec3(0.0, -100.5, -1.0));*/

	/*
	shader.setVec3("tri.v0", glm::vec3(0.0, 0.0, -1.0));
	shader.setVec3("tri.v1", glm::vec3(1.0, 0.0, -1.0));
	shader.setVec3("tri.v2", glm::vec3(-1.0, 1.0, -1.0));
	*/
}