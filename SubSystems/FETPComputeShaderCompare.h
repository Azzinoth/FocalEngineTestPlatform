#pragma once

#include "FETPScreenCapture.h"

static const char* const ComputeShaderText = R"(
	#version 430

	uniform sampler2D u_screenshotTexture;
	uniform sampler2D u_subImageTexture;
	uniform ivec2 u_screenshotSize;
	uniform ivec2 u_subImageSize;
	uniform float u_matchPercentage;
	uniform float u_colorTolerance;

	layout (std430, binding = 0) buffer Result
	{
		uint match_found;
		uint match_position[2];
	};

	layout (local_size_x = 16, local_size_y = 16) in;

	bool compareColors(vec4 a, vec4 b, float tolerance)
	{
		return all(lessThanEqual(abs(a - b), vec4(tolerance)));
	}

	void main()
	{
		ivec2 global_id = ivec2(gl_GlobalInvocationID.xy);

		if (global_id.x + u_subImageSize.x <= u_screenshotSize.x &&
			global_id.y + u_subImageSize.y <= u_screenshotSize.y)
		{
			if (match_found == 1)
				return;

			uint totalPixels = 0u;
			uint matchPixels = 0u;

			for (int y = 0; y < u_subImageSize.y; y++)
			{
				for (int x = 0; x < u_subImageSize.x; x++)
				{
					vec4 screenshotColor = texelFetch(u_screenshotTexture, global_id + ivec2(x, y), 0);
					vec4 subImageColor = texelFetch(u_subImageTexture, ivec2(x, y), 0);
					totalPixels++;

					if (compareColors(screenshotColor, subImageColor, u_colorTolerance))
					{
						matchPixels++;
					}
				}
			}

			float matchRatio = float(matchPixels) / float(totalPixels);

			if (matchRatio >= u_matchPercentage)
			{
				atomicExchange(match_found, 1);
				atomicExchange(match_position[0], global_id.x);
				atomicExchange(match_position[1], global_id.y);
			}
		}
	}
)";

class FETPComputeShaderCompare
{
public:
	SINGLETON_PUBLIC_PART(FETPComputeShaderCompare)

	glm::vec2 FindSubImage(FETPImage* ScreenTexture, FETPImage* ImageToFind, float correctnessThreshold, int maxColorShift);
private:
	SINGLETON_PRIVATE_PART(FETPComputeShaderCompare)

	struct ComparisonResult
	{
		GLuint MatchFound;
		GLuint MatchPosition[2];
	};

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

#define COMPUTE_SHADER_COMPARE FETPComputeShaderCompare::getInstance()