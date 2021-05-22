#pragma once
#include "IETThread.h"
#include "GL/glew.h"

class IExecutionEvent;
class SkyboxLoader : public IETThread
{
public:
	SkyboxLoader(IExecutionEvent* execEvent, GLuint* renderingProgram);
	~SkyboxLoader();

private:
	void run() override;
	IExecutionEvent* execEvent;
	GLuint* skyboxRenderingProgram;
};

