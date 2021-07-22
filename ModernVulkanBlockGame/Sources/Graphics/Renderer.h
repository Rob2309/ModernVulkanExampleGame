#pragma once

#include "Window.h"

namespace Graphics::Renderer {

	/// <summary>
	/// Initializes the Renderer
	/// </summary>
	///	<remarks>Must be called after Manager::Initialize()</remarks>
	void Initialize();
	/// <summary>
	/// Deinitializes the Renderer
	/// </summary>
	///	<remarks>Must be called before Manager::Terminate()</remarks>
	void Terminate();

	/// <summary>
	/// Renders a single frame to a given window.
	/// </summary>
	/// <param name="wnd">The window to render to</param>
	void RenderFrame(Window& wnd);

	/// <summary>
	/// The maximum number of frames the CPU may be ahead of the GPU.
	/// </summary>
	inline constexpr int MAX_FRAMES_IN_FLIGHT = 3;

}
