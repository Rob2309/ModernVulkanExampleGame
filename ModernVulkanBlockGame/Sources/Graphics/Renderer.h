#pragma once

#include "Window.h"

namespace Graphics::Renderer {

	void Initialize();
	void Terminate();

	void RenderFrame(Window& wnd);

	inline constexpr int MAX_FRAMES_IN_FLIGHT = 3;

}
