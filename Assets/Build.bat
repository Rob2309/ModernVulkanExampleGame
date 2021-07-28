@echo off

for %%f in (%~dp0Shaders\*.hlsl) do (
    echo Compiling Vulkan HLSL shader %%f
    glslc --target-env=vulkan1.2 -fauto-combined-image-sampler -fshader-stage=vert -fentry-point=vert %%f -o %%~dpnf.vert.spv
    glslc --target-env=vulkan1.2 -fauto-combined-image-sampler -fshader-stage=frag -fentry-point=frag %%f -o %%~dpnf.frag.spv
)
