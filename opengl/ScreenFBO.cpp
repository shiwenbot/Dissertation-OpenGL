#include "ScreenFBO.h"

ScreenFBO::ScreenFBO(){}
void ScreenFBO::Config(int SCR_WIDTH, int SCR_HEIGHT) {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// 绑定颜色纹理
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);//分配内存，指定格式大小
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	unBind();
}
void ScreenFBO::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glDisable(GL_DEPTH_TEST);
}
void ScreenFBO::unBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ScreenFBO::BindAsTexture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
}
void ScreenFBO::Delete() {
	unBind();
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &textureColorbuffer);
}

//=========================================================//
FBOManager::FBOManager(int SCR_WIDTH, int SCR_HEIGHT){
	fbo[0].Config(SCR_WIDTH, SCR_HEIGHT);
	fbo[1].Config(SCR_WIDTH, SCR_HEIGHT);
}

// fbo[0]为当前帧
void FBOManager::Bind(int LoopNum) {
	int histIndex = LoopNum % 2;
	int curIndex = (histIndex == 0 ? 1 : 0);

	fbo[curIndex].Bind();
	fbo[histIndex].BindAsTexture();
}

void FBOManager::BindAsTexture(int LoopNum) {
	int histIndex = LoopNum % 2;
	int curIndex = (histIndex == 0 ? 1 : 0);
	fbo[curIndex].BindAsTexture();
}
