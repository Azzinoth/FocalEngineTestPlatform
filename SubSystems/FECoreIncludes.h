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