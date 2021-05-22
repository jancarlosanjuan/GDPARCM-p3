#include "SkyboxLoader.h"
#include "IExecutionEvent.h"
//#include "ShaderProgramAttachment.h"

SkyboxLoader::SkyboxLoader(IExecutionEvent* execEvent, GLuint* renderingProgram)
{
	this->execEvent = execEvent;
	this->skyboxRenderingProgram = renderingProgram;
}

SkyboxLoader::~SkyboxLoader()
{
}

void SkyboxLoader::run()
{
	//*skyboxRenderingProgram = createSkyboxShaderProgram();
	this->execEvent->OnFinishedExecution();
	delete this;
}
