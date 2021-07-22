#include "Logging/Log.h"
#include "Graphics/Manager.h"
#include "Graphics/Renderer.h"
#include "Graphics/Window.h"

int main() {
	Log::Info("Initializing Graphics System");
	if(!Graphics::Manager::Initialize()) {
		Log::Error("Failed to initialize Graphics System, exiting");
		return 1;
	}

	Log::Info("Initializing Renderer");
	Graphics::Renderer::Initialize();

	Log::Info("Creating window");
	Graphics::Window wnd{ 800, 600, "Modern Vulkan Block Game" };

	while(!wnd.Closed()) {
		Graphics::Window::UpdateAll();
		Graphics::Renderer::RenderFrame(wnd);
	}

	Graphics::Manager::WaitIdle();

	Log::Info("Destroying Window");
	wnd.Destroy();

	Log::Info("Terminating Renderer");
	Graphics::Renderer::Terminate();

	Log::Info("Terminating Graphics System");
	Graphics::Manager::Terminate();
}
