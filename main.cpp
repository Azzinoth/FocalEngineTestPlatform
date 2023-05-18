#include "Windows/testsOverviewWindow.h"

// Shader related stuff
GLuint ProgramID;
GLuint ComputeShaderID;
GLint screenshotSizeLocation;
GLint subImageSizeLocation;
GLint toleranceLocation;

GLuint MatchBuffer;

FETPImage* SubImage = nullptr;

const GLuint workGroupSizeX = 16;
const GLuint workGroupSizeY = 16;

static const char* const ComputeShaderTestText = R"(
	#version 430
	
	uniform sampler2D u_screenshotTexture;
	uniform ivec2 u_screenshotSize;


	layout (std430, binding = 0) buffer Result
	{
		int match_found;
		ivec2 match_position;
	};

	layout (local_size_x = 16, local_size_y = 16) in;
	
)";

static const char* const ComputeShaderText = R"(
	#version 430

	uniform sampler2D u_screenshotTexture;
	uniform sampler2D u_subImageTexture;
	uniform ivec2 u_screenshotSize;
	uniform ivec2 u_subImageSize;
	uniform float u_tolerance;

	layout (std430, binding = 0) buffer Result
	{
		int match_found;
		ivec2 match_position;
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
			bool match = true;

			for (int y = 0; y < u_subImageSize.y && match; y++)
			{
				for (int x = 0; x < u_subImageSize.x && match; x++)
				{
					vec4 screenshotColor = texelFetch(u_screenshotTexture, global_id + ivec2(x, y), 0);
					vec4 subImageColor = texelFetch(u_subImageTexture, ivec2(x, y), 0);
					if (!compareColors(screenshotColor, subImageColor, u_tolerance))
					{
						match = false;
					}
				}
			}

			if (match)
			{
				atomicExchange(match_found, 1);
				atomicExchange(match_position.x, global_id.x);
				atomicExchange(match_position.y, global_id.y);
			}
		}
	}
)";


// Shader related stuff END

void keyButtonCallback(int key, int scancode, int action, int mods)
{
	if (key == 84 && action == GLFW_RELEASE && TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (textInputPopup::getInstance().isOpened())
			return;

		ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());

		while (TEST_MANAGER.getSelectedTest()->getLoopCount() > 1)
		{
			TEST_MANAGER.getSelectedTest()->setLoopCount(TEST_MANAGER.getSelectedTest()->getLoopCount() - 1);
			ACTION_SYSTEM.run(TEST_MANAGER.getSelectedTest());
		}
	}
}

void globalKeyButtonsCallback(KeyboardAction keyAction)
{
	ACTION_SYSTEM.newKeyboardAction(keyAction);
}

void globalMouseCallback(MouseAction mouseAction)
{
	ACTION_SYSTEM.newMouseAction(mouseAction);
}

GLuint LoadShader(const char* ShaderText, const GLuint ShaderType)
{
	GLuint ShaderID;
	FE_GL_ERROR(ShaderID = glCreateShader(ShaderType));

	//const std::string TempString = ParseShaderForMacro(ShaderText);
	//const char* ParsedShaderText = TempString.c_str();
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

	GLuint ProgramID;
	FE_GL_ERROR(ProgramID = glCreateProgram());
	FE_GL_ERROR(glAttachShader(ProgramID, ShaderID));

	FE_GL_ERROR(glLinkProgram(ProgramID));
	FE_GL_ERROR(glValidateProgram(ProgramID));

	return ProgramID;
}


void TestShaderLoading()
{
	ComputeShaderID = LoadShader(ComputeShaderText, GL_COMPUTE_SHADER);
	FE_GL_ERROR(glUseProgram(ComputeShaderID));

	//GLint Count = 0;
	//FE_GL_ERROR(glGetProgramiv(ComputeShaderID, GL_ACTIVE_UNIFORMS, &Count));

	screenshotSizeLocation = glGetUniformLocation(ComputeShaderID, "u_screenshotSize");
	subImageSizeLocation = glGetUniformLocation(ComputeShaderID, "u_subImageSize");
	toleranceLocation = glGetUniformLocation(ComputeShaderID, "u_tolerance");

	glGenBuffers(1, &MatchBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, MatchBuffer);

	// Initialize the buffer with 0 for 'match_found' and (-1, -1) for 'match_position'
	struct {
		GLint match_found;
		GLint match_position[2];
	} initialData = { 0, {-1, -1} };

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(initialData), &initialData, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, MatchBuffer);

	SubImage = new FETPImage("testSub.png");
}

void RunComputeShader(FETPImage* ScreenTexture, FETPImage* ImageToFind)
{
	FE_GL_ERROR(glUseProgram(ComputeShaderID));

	glUniform2i(screenshotSizeLocation, ScreenTexture->getWidth(), ScreenTexture->getHeight());
	glUniform2i(subImageSizeLocation, ImageToFind->getWidth(), ImageToFind->getHeight());
	glUniform1f(toleranceLocation, 1.0f);

	FE_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 0));
	FE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ScreenTexture->getTextureID()));

	FE_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 1));
	FE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ImageToFind->getTextureID()));


	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, MatchBuffer);
	int DispatchSizeX = (ScreenTexture->getWidth() - ImageToFind->getWidth() + workGroupSizeX) / workGroupSizeX + 1;
	int DispatchSizeY = (ScreenTexture->getHeight() - ImageToFind->getHeight() + workGroupSizeY) / workGroupSizeY + 1;
	FE_GL_ERROR(glDispatchCompute(DispatchSizeX, DispatchSizeY, 1));


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	struct {
		GLint match_found;
		GLint match_position[2];
	} resultData;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, MatchBuffer);
	void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	memcpy(&resultData, ptr, sizeof(resultData));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	TEST_PLATFORM.createWindow();
	TEST_PLATFORM.setKeyboardCallback(keyButtonCallback);

	INPUT_SYSTEM.initialize();
	INPUT_SYSTEM.setGlobalKeyboardCallback(globalKeyButtonsCallback);
	INPUT_SYSTEM.setGlobalMouseCallback(globalMouseCallback);

	testsOverviewWindow::getInstance().show();
	testEditorWinow::getInstance().show();
	testPropertiesWindow::getInstance().show();

	TestShaderLoading();

	TIME.BeginTimeStamp("M");
	FETPImage* ScreenImage = new FETPImage("test.png");
	RunComputeShader(ScreenImage, SubImage);
	auto time = TIME.EndTimeStamp("M");

	while (TEST_PLATFORM.isWindowOpened())
	{
		TEST_PLATFORM.beginFrame();
		INPUT_SYSTEM.update();
		
		testsOverviewWindow::getInstance().render();
		testEditorWinow::getInstance().render();
		testPropertiesWindow::getInstance().render();
		testStartPreparationsWindow::getInstance().render();
		nodeRegionWindow::getInstance().render();

		previewWindow::getInstance().render();
		failedTestWindow::getInstance().render();
		screenshootEditor::getInstance().render();

		textInputPopup::getInstance().render();
		actionEditPopup::getInstance().render();
		ACTION_SYSTEM.update();



		//TIME.BeginTimeStamp("M");
		//FETPImage* TestScreenShoot = SCREEN_SYSTEM.GetScreenDataAsImage();
		////delete TestScreenShoot;
		//auto time = TIME.EndTimeStamp("M");
		//TEST_PLATFORM.setWindowTitle(std::to_string(time));

		//RunComputeShader(TestScreenShoot, SubImage);



		/*auto Temp = TestScreenShoot->getRawData();
		lodepng::encode("test.png", Temp, TestScreenShoot->getWidth(), TestScreenShoot->getHeight());
		delete TestScreenShoot;
		delete[] Temp;*/

		/*TIME.BeginTimeStamp("M");
		unsigned char* TestScreenShoot = SCREEN_SYSTEM.getScreenData();
		auto time = TIME.EndTimeStamp("M");
		TEST_PLATFORM.setWindowTitle(std::to_string(time));*/
		
		

		/*std::vector<unsigned char> tempScreenshoot;
		tempScreenshoot.resize(512 * 512 * 4);
		Sleep(500);
		SCREEN_SYSTEM.getScreenRegion(tempScreenshoot.data(), 0, 0, 512, 512);

		int similarity = SCREEN_SYSTEM.compare(512, 512, tempScreenshoot.data(), image->getRawData(), tempDifferenceData.data());*/
		//SCREEN_SYSTEM.updateScreenData();

		//textLOG.clear();
		//for (size_t i = 0; i < recordedActions.size(); i++)
		//{
		//	if (recordedActions[i].getActionType() != 0)
		//		continue;

		//	if (recordedActions[i].keyboard.wParam == WM_KEYDOWN || recordedActions[i].keyboard.wParam == WM_SYSKEYDOWN)
		//	{
		//		textLOG += "key down:" + std::to_string(recordedActions[i].keyboard.additionalInfo.vkCode) + "\n";
		//	}
		//	else
		//	{
		//		textLOG += "key up:" + std::to_string(recordedActions[i].keyboard.additionalInfo.vkCode) + "\n";
		//	}
		//}

		//
		//strcpy_s(tempText, textLOG.size() + 1, textLOG.c_str());
		//ImGui::InputTextMultiline("LOG", tempText, strlen(tempText));

		//ImGui::ShowDemoWindow();

		//bool shiftPressed = HIWORD(GetKeyState(VK_SHIFT)) || HIWORD(GetKeyState(VK_LSHIFT)) || HIWORD(GetKeyState(VK_RSHIFT));
		//bool capsLockActivated = LOWORD(GetKeyState(VK_CAPITAL));

		//if (shiftPressed || capsLockActivated)
		//{
		//	int y = 0;
		//	y++;
		//}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					return 0;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleVar();

		//TEST_PLATFORM.setWindowTitle();
		TEST_PLATFORM.endFrame();
	}

	return 0;
}