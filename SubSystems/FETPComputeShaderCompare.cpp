#include "FETPComputeShaderCompare.h"

FETPComputeShaderCompare* FETPComputeShaderCompare::Instance = nullptr;

FETPComputeShaderCompare::FETPComputeShaderCompare()
{
    LoadShader(ComputeShaderText, GL_COMPUTE_SHADER);
	FE_GL_ERROR(glUseProgram(ProgramID));

	ScreenshotSizeLocation = glGetUniformLocation(ProgramID, "u_screenshotSize");
	SubImageSizeLocation = glGetUniformLocation(ProgramID, "u_subImageSize");
	MatchPercentageLocation = glGetUniformLocation(ProgramID, "u_matchPercentage");
	ColorToleranceLocation = glGetUniformLocation(ProgramID, "u_colorTolerance");

	FE_GL_ERROR(glUniform1i(glGetUniformLocation(ProgramID, "u_screenshotTexture"), 0));
	FE_GL_ERROR(glUniform1i(glGetUniformLocation(ProgramID, "u_subImageTexture"), 1));

	FE_GL_ERROR(glGenBuffers(1, &MatchBuffer));
	FE_GL_ERROR(glBindBuffer(GL_SHADER_STORAGE_BUFFER, MatchBuffer));

	ComparisonResult InitialData;
	InitialData.MatchFound = 0;
	InitialData.MatchPosition[0] = -1;
	InitialData.MatchPosition[1] = -1;

	FE_GL_ERROR(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(InitialData), &InitialData, GL_DYNAMIC_DRAW));
	FE_GL_ERROR(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, MatchBuffer));
}

GLuint FETPComputeShaderCompare::LoadShader(const char* ShaderText, const GLuint ShaderType)
{
	GLuint ShaderID;
	FE_GL_ERROR(ShaderID = glCreateShader(ShaderType));

	FE_GL_ERROR(glShaderSource(ShaderID, 1, &ShaderText, nullptr));
	FE_GL_ERROR(glCompileShader(ShaderID));
	GLint status = 0;
	FE_GL_ERROR(glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &status));

	std::string CompilationErrors;
	if (status == GL_FALSE) {
		GLint LogSize = 0;
		FE_GL_ERROR(glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogSize));
		std::vector<GLchar> ErrorLog(LogSize);

		FE_GL_ERROR(glGetShaderInfoLog(ShaderID, LogSize, &LogSize, &ErrorLog[0]));
		for (size_t i = 0; i < ErrorLog.size(); i++)
		{
			CompilationErrors.push_back(ErrorLog[i]);
		}

		assert(status);
	}

	FE_GL_ERROR(ProgramID = glCreateProgram());
	FE_GL_ERROR(glAttachShader(ProgramID, ShaderID));

	FE_GL_ERROR(glLinkProgram(ProgramID));
	FE_GL_ERROR(glValidateProgram(ProgramID));

	return ProgramID;
}

glm::vec2 FETPComputeShaderCompare::FindSubImage(FETPImage* ScreenTexture, FETPImage* ImageToFind, float correctnessThreshold, int maxColorShift)
{
	glm::vec2 Result = glm::vec2(-1, -1);
	FE_GL_ERROR(glUseProgram(ProgramID));

	FE_GL_ERROR(glUniform2i(ScreenshotSizeLocation, ScreenTexture->GetWidth(), ScreenTexture->GetHeight()));
	FE_GL_ERROR(glUniform2i(SubImageSizeLocation, ImageToFind->GetWidth(), ImageToFind->GetHeight()));
	FE_GL_ERROR(glUniform1f(MatchPercentageLocation, correctnessThreshold / 100.0f));
	FE_GL_ERROR(glUniform1f(ColorToleranceLocation, maxColorShift / 100.0f));

	FE_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 0));
	FE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ScreenTexture->GetTextureID()));

	FE_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 1));
	FE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ImageToFind->GetTextureID()));


	FE_GL_ERROR(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, MatchBuffer));
	int DispatchSizeX = (ScreenTexture->GetWidth() - ImageToFind->GetWidth() + WorkGroupSizeX) / WorkGroupSizeX + 1;
	int DispatchSizeY = (ScreenTexture->GetHeight() - ImageToFind->GetHeight() + WorkGroupSizeY) / WorkGroupSizeY + 1;
	FE_GL_ERROR(glDispatchCompute(DispatchSizeX, DispatchSizeY, 1));
	//FE_GL_ERROR(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	FE_GL_ERROR(glMemoryBarrier(GL_ALL_BARRIER_BITS));

	FE_GL_ERROR(glBindBuffer(GL_SHADER_STORAGE_BUFFER, MatchBuffer));
	void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	ComparisonResult* ResultData = (ComparisonResult*)ptr;
	if (ResultData->MatchFound)
		Result = glm::vec2(ResultData->MatchPosition[0], ResultData->MatchPosition[1]);

	ResultData->MatchFound = 0;
	ResultData->MatchPosition[0] = 0;
	ResultData->MatchPosition[1] = 0;

	FE_GL_ERROR(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));

	return Result;
}