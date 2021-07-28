
CXX := g++
CXXFLAGS := -std=c++20 -I./Sources -DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 -static-libstdc++
LDFLAGS := -std=c++20 -lfmt -lvulkan -lglfw -static-libstdc++

cpp_sources := $(shell find ./Sources -type f -name "*.cpp" -printf "%p ")
cpp_objects := $(patsubst ./Sources/%.cpp,../Build/Intermediates/ModernVulkanBlockGame/%.cpp.o,$(cpp_sources))

../Build/Bin/ModernVulkanBlockGame: $(cpp_objects)
	@ mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDFLAGS)

../Build/Intermediates/ModernVulkanBlockGame/%.cpp.o: ./Sources/%.cpp
	@ mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MP -MF $(@:.cpp.o=.cpp.d)

-include $(cpp_objects:.cpp.o=.cpp.d)
