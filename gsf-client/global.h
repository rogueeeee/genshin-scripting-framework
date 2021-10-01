#pragma once

#include <d3d11.h>

namespace global
{
	inline void *dll_handle = nullptr;
	inline void *game_window = nullptr;

	inline bool cursor_is_visible = true;

	inline ID3D11DeviceContext    *dx_context            = nullptr;
	inline ID3D11RenderTargetView *dx_render_target_view = nullptr;
}