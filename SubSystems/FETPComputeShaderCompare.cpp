#include "FETPComputeShaderCompare.h"

FETPComputeShaderCompare::FETPComputeShaderCompare()
{
    LoadShader(ComputeShaderText, GL_COMPUTE_SHADER);
	FE_GL_ERROR(glUseProgram(ProgramID));

	ScreenshotSizeLocation = glGetUniformLocation(ProgramID, "ScreenshotSize");
	SubImageSizeLocation = glGetUniformLocation(ProgramID, "SubImageSize");
	MatchPercentageLocation = glGetUniformLocation(ProgramID, "MatchPercentage");
	ColorToleranceLocation = glGetUniformLocation(ProgramID, "ColorTolerance");

	FE_GL_ERROR(glUniform1i(glGetUniformLocation(ProgramID, "ScreenshotTexture"), 0));
	FE_GL_ERROR(glUniform1i(glGetUniformLocation(ProgramID, "SubImageTexture"), 1));

	FE_GL_ERROR(glGenBuffers(1, &MatchBuffer));
	FE_GL_ERROR(glBindBuffer(GL_SHADER_STORAGE_BUFFER, MatchBuffer));

	ComparisonResult InitialData;
	InitialData.MatchFound = 0;
	InitialData.MatchPosition[0] = -1;
	InitialData.MatchPosition[1] = -1;
	InitialData.BestMatchScore = 0;
	InitialData.BestMatchPosition[0] = -1;
	InitialData.BestMatchPosition[1] = -1;

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

FETPComputeShaderCompare::ComparisonResult FETPComputeShaderCompare::FindSubImage(FETPImage* ScreenTexture, FETPImage* ImageToFind, float CorrectnessThreshold, int MaxColorShift)
{
	ComparisonResult Result = {};
	Result.MatchPosition[0] = static_cast<GLuint>(-1);
	Result.MatchPosition[1] = static_cast<GLuint>(-1);
	Result.BestMatchPosition[0] = static_cast<GLuint>(-1);
	Result.BestMatchPosition[1] = static_cast<GLuint>(-1);

	if (ScreenTexture == nullptr || ImageToFind == nullptr)
		return Result;

	if (ImageToFind->GetWidth() <= 0 || ImageToFind->GetHeight() <= 0 ||
		ImageToFind->GetWidth() > ScreenTexture->GetWidth() ||
		ImageToFind->GetHeight() > ScreenTexture->GetHeight())
		return Result;

	FE_GL_ERROR(glUseProgram(ProgramID));

	FE_GL_ERROR(glUniform2i(ScreenshotSizeLocation, ScreenTexture->GetWidth(), ScreenTexture->GetHeight()));
	FE_GL_ERROR(glUniform2i(SubImageSizeLocation, ImageToFind->GetWidth(), ImageToFind->GetHeight()));
	FE_GL_ERROR(glUniform1f(MatchPercentageLocation, CorrectnessThreshold / 100.0f));
	FE_GL_ERROR(glUniform1f(ColorToleranceLocation, MaxColorShift / 100.0f));

	FE_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 0));
	FE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ScreenTexture->GetTextureID()));

	FE_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 1));
	FE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ImageToFind->GetTextureID()));

	FE_GL_ERROR(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, MatchBuffer));
	int DispatchSizeX = (ScreenTexture->GetWidth() - ImageToFind->GetWidth() + WorkGroupSizeX) / WorkGroupSizeX + 1;
	int DispatchSizeY = (ScreenTexture->GetHeight() - ImageToFind->GetHeight() + WorkGroupSizeY) / WorkGroupSizeY + 1;

	FE_GL_ERROR(glDispatchCompute(DispatchSizeX, DispatchSizeY, 1));
	FE_GL_ERROR(glMemoryBarrier(GL_ALL_BARRIER_BITS));

	FE_GL_ERROR(glBindBuffer(GL_SHADER_STORAGE_BUFFER, MatchBuffer));
	void* BufferPointer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	ComparisonResult* ResultData = (ComparisonResult*)BufferPointer;
	Result.MatchFound = ResultData->MatchFound;
	if (ResultData->MatchFound)
	{
		Result.MatchPosition[0] = ResultData->MatchPosition[0];
		Result.MatchPosition[1] = ResultData->MatchPosition[1];
	}
	Result.BestMatchScore = ResultData->BestMatchScore;
	Result.BestMatchPosition[0] = ResultData->BestMatchPosition[0];
	Result.BestMatchPosition[1] = ResultData->BestMatchPosition[1];

	ResultData->MatchFound = 0;
	ResultData->MatchPosition[0] = 0;
	ResultData->MatchPosition[1] = 0;
	ResultData->BestMatchScore = 0;
	ResultData->BestMatchPosition[0] = -1;
	ResultData->BestMatchPosition[1] = -1;

	FE_GL_ERROR(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));

	return Result;
}

FETPComputeShaderCompare::ComparisonResult FETPComputeShaderCompare::FindSubImageOnCPU(FETPImage* ScreenTexture, FETPImage* ImageToFind, float CorrectnessThreshold, int MaxColorShift)
{
	ComparisonResult Result = {};
	Result.MatchFound = 0;
	Result.MatchPosition[0] = static_cast<GLuint>(-1);
	Result.MatchPosition[1] = static_cast<GLuint>(-1);
	Result.BestMatchScore = 0;
	Result.BestMatchPosition[0] = static_cast<GLuint>(-1);
	Result.BestMatchPosition[1] = static_cast<GLuint>(-1);

	if (ScreenTexture == nullptr || ImageToFind == nullptr)
		return Result;

	int ScreenshotWidth = ScreenTexture->GetWidth();
	int ScreenshotHeight = ScreenTexture->GetHeight();
	int SubImageWidth = ImageToFind->GetWidth();
	int SubImageHeight = ImageToFind->GetHeight();

	if (SubImageWidth <= 0 || SubImageHeight <= 0 ||
		SubImageWidth > ScreenshotWidth || SubImageHeight > ScreenshotHeight)
		return Result;

	unsigned char* ScreenshotData = ScreenTexture->GetRawData();
	unsigned char* SubImageData = ImageToFind->GetRawData();
	if (ScreenshotData == nullptr || SubImageData == nullptr)
	{
		delete[] ScreenshotData;
		delete[] SubImageData;
		return Result;
	}

	float MatchPercentage = CorrectnessThreshold / 100.0f;
	float ColorTolerance = MaxColorShift / 100.0f;

	auto CompareColors = [](const unsigned char* PixelA, const unsigned char* PixelB, float Tolerance) -> bool
	{
		for (int Channel = 0; Channel < 4; Channel++)
		{
			float Difference = std::abs(float(PixelA[Channel]) - float(PixelB[Channel])) / 255.0f;
			if (Difference > Tolerance)
				return false;
		}
		return true;
	};

	for (int OriginY = 0; OriginY <= ScreenshotHeight - SubImageHeight; OriginY++)
	{
		for (int OriginX = 0; OriginX <= ScreenshotWidth - SubImageWidth; OriginX++)
		{
			if (Result.MatchFound == 1)
				break;

			unsigned int TotalPixels = 0;
			unsigned int MatchedPixels = 0;

			for (int Y = 0; Y < SubImageHeight; Y++)
			{
				for (int X = 0; X < SubImageWidth; X++)
				{
					const unsigned char* ScreenshotColor = &ScreenshotData[((OriginY + Y) * ScreenshotWidth + (OriginX + X)) * 4];
					const unsigned char* SubImageColor = &SubImageData[(Y * SubImageWidth + X) * 4];
					TotalPixels++;
					if (CompareColors(ScreenshotColor, SubImageColor, ColorTolerance))
						MatchedPixels++;
				}
			}

			float MatchRatio = float(MatchedPixels) / float(TotalPixels);
			GLuint Score = GLuint(MatchRatio * 1000000.0f);
			if (Score > Result.BestMatchScore)
			{
				Result.BestMatchScore = Score;
				Result.BestMatchPosition[0] = GLuint(OriginX);
				Result.BestMatchPosition[1] = GLuint(OriginY);
			}

			if (MatchRatio >= MatchPercentage)
			{
				Result.MatchFound = 1;
				Result.MatchPosition[0] = GLuint(OriginX);
				Result.MatchPosition[1] = GLuint(OriginY);
			}
		}

		if (Result.MatchFound == 1)
			break;
	}

	delete[] ScreenshotData;
	delete[] SubImageData;
	return Result;
}