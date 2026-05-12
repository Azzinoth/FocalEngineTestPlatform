#pragma once

#include "FEBasicApplication.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define GLM_FORCE_XYZW_ONLY
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.inl"
#include "glm/gtx/quaternion.hpp"

#include "jsoncpp/json/json.h"

#include <stdlib.h>

#include <thread>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <winuser.h>

#include <shellapi.h>

#include "../ThirdParty/lodepng/lodepng.h"

#define FE_WIN_32

#ifdef FE_DEBUG_ENABLED
#define FE_GL_ERROR(glCall)                \
	{                                      \
		glCall;                            \
		GLenum error = glGetError();	   \
		if (error != 0)                    \
		{								   \
			assert("FE_GL_ERROR" && 0);	   \
		}                                  \
	}
#else
#define FE_GL_ERROR(glCall)                \
	{                                      \
		glCall;                            \
	}
#endif // FE_GL_ERROR