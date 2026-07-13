#pragma once

#include "FETPScreenCapture.h"

static const char* const ComputeShaderText = R"(
	#version 430

	uniform sampler2D ScreenshotTexture;
	uniform sampler2D SubImageTexture;
	uniform ivec2 ScreenshotSize;
	uniform ivec2 SubImageSize;
	uniform float MatchPercentage;
	uniform float ColorTolerance;

	layout (std430, binding = 0) buffer Result
	{
		uint MatchFound;
		uint MatchPosition[2];
		uint BestMatchScore;
		uint BestMatchPosition[2];
	};

	layout (local_size_x = 16, local_size_y = 16) in;

	bool CompareColors(vec4 ColorA, vec4 ColorB, float Tolerance)
	{
		return all(lessThanEqual(abs(ColorA - ColorB), vec4(Tolerance)));
	}

	void main()
	{
		ivec2 GlobalID = ivec2(gl_GlobalInvocationID.xy);

		if (GlobalID.x + SubImageSize.x <= ScreenshotSize.x &&
			GlobalID.y + SubImageSize.y <= ScreenshotSize.y)
		{
			if (MatchFound == 1)
				return;

			uint TotalPixels = 0u;
			uint MatchedPixels = 0u;

			for (int Y = 0; Y < SubImageSize.y; Y++)
			{
				for (int X = 0; X < SubImageSize.x; X++)
				{
					vec4 ScreenshotColor = texelFetch(ScreenshotTexture, GlobalID + ivec2(X, Y), 0);
					vec4 SubImageColor = texelFetch(SubImageTexture, ivec2(X, Y), 0);
					TotalPixels++;

					if (CompareColors(ScreenshotColor, SubImageColor, ColorTolerance))
					{
						MatchedPixels++;
					}
				}
			}

			float MatchRatio = float(MatchedPixels) / float(TotalPixels);

			uint Score = uint(MatchRatio * 1000000.0);
			uint PreviousBestScore = atomicMax(BestMatchScore, Score);
			if (Score > PreviousBestScore)
			{
				atomicExchange(BestMatchPosition[0], uint(GlobalID.x));
				atomicExchange(BestMatchPosition[1], uint(GlobalID.y));
			}

			if (MatchRatio >= MatchPercentage)
			{
				atomicExchange(MatchFound, 1);
				atomicExchange(MatchPosition[0], GlobalID.x);
				atomicExchange(MatchPosition[1], GlobalID.y);
			}
		}
	}
)";

class FETPComputeShaderCompare
{
public:
	SINGLETON_PUBLIC_PART(FETPComputeShaderCompare)

	struct ComparisonResult
	{
		GLuint MatchFound;
		GLuint MatchPosition[2];
		GLuint BestMatchScore;
		GLuint BestMatchPosition[2];
	};

	ComparisonResult FindSubImage(FETPImage* ScreenTexture, FETPImage* ImageToFind, float CorrectnessThreshold, int MaxColorShift);
	ComparisonResult FindSubImageOnCPU(FETPImage* ScreenTexture, FETPImage* ImageToFind, float CorrectnessThreshold, int MaxColorShift);
private:
	SINGLETON_PRIVATE_PART(FETPComputeShaderCompare)

    GLuint ProgramID;
	GLint ScreenshotSizeLocation;
	GLint SubImageSizeLocation;
	GLint MatchPercentageLocation;
	GLint ColorToleranceLocation;

	GLuint MatchBuffer;

	FETPImage* SubImage = nullptr;
	FETPImage* CanvasImage = nullptr;

	const GLuint WorkGroupSizeX = 16;
	const GLuint WorkGroupSizeY = 16;
	
	GLuint LoadShader(const char* ShaderText, const GLuint ShaderType);
};

#define COMPUTE_SHADER_COMPARE FETPComputeShaderCompare::GetInstance()