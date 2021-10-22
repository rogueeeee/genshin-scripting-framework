#pragma once

#include "enums.h"

#include "unity_object.h"
#include "unity_component.h"
#include "unity_transform.h"
#include "unity_vector3.h"
#include "unity_camera.h"
#include "unity_animator.h"
#include "unity_scripting_api.h"

#include "../il2cpp/il2cpp.h"
#include <vector>
#include <macro.h>
#include <d3d11.h>

namespace game::sdk
{
	using get_dx_swapchain_t = IDXGISwapChain *(*)(void);
	using get_dx_devicectx_t = ID3D11DeviceContext *(*)(void);

	using Boolean = int;
	using Type    = void;
}