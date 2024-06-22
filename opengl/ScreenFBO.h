#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
class ScreenFBO {
private:
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
public:
	ScreenFBO();
	void Config(int SCR_WIDTH, int SCR_HEIGHT);
	void Bind();
	void unBind(); // Bind to default framebuffer
	void BindAsTexture();
	void Delete();
};

class FBOManager {
private:
	int currentIndex = 0;
	ScreenFBO fbo[2];
public:
	FBOManager(int SCR_WIDTH, int SCR_HEIGHT);
	void Bind(int LoopNum);
	void BindAsTexture(int LoopNum);
	void Delete() {
		fbo[0].Delete();
		fbo[1].Delete();
	}
};