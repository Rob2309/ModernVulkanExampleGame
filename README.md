# Modern Vulkan Example Project

This repository contains an example project using modern Vulkan (1.2+). The goal is to build a simple 3D Game while providing as much in-code documentation as possible, hopefully making understanding an actual Vulkan application as easy as possible. If concepts don't get clear enough through the in-code comments, feel free to ask questions or suggest edits.

*Disclaimer: I am in no way an expert on Vulkan, I am merely a mortal human who thinks I have encountered/solved enough problems, read the spec enough times and worked through enough tutorials to help others.*

Concepts I plan to touch on currently include:
- Basic rendering concepts (Window creation, swapchain, RenderPass, CommandBuffer, ...)
- Background data transfer (e.g. using async transfer queues on Desktop hardware)
- multi-threaded CommandBuffer recording
- "Real" HDR rendering (using a HDR swapchain)
- Compute Shaders
- ...

Feel free to suggest features that you are interested in.

## Building and running
To build on windows, just open the Visual Studio solution and build/run the project.

Building on Linux requires g++-11 or a compatible clang version (not tested).
On Ubuntu, install the following packages:
`sudo apt install libglfw3-dev libfmt-dev vulkan-sdk`.
On other distros, find the corresponding packages.
Then run `make build` or `make run` from the root folder.

## Useful resources
- Vulkan Spec: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/index.html
- Vulkan examples: https://github.com/SaschaWillems/Vulkan

## Third-Party code
The [ThirdParty](ThirdParty) folder contains all code that is not written by me. For the respective Licenses, see each folder.
