
GLSLC := glslc

hlsl_sources := $(shell find ./Shaders -type f -name "*.hlsl" -printf "%p ")
spv_objects := $(patsubst ./Shaders/%.hlsl,./Shaders/%.vert.spv,$(hlsl_sources)) $(patsubst ./Shaders/%.hlsl,./Shaders/%.frag.spv,$(hlsl_sources))

shaders: $(spv_objects)

./Shaders/%.vert.spv: ./Shaders/%.hlsl
	$(GLSLC) --target-env=vulkan1.2 -fauto-combined-image-sampler -fshader-stage=vert -fentry-point=vert $< -o $@

./Shaders/%.frag.spv: ./Shaders/%.hlsl
	$(GLSLC) --target-env=vulkan1.2 -fauto-combined-image-sampler -fshader-stage=frag -fentry-point=frag $< -o $@

.PHONY: clean
clean:
	rm -f ./Shaders/*.spv
