
.PHONY: build
build:
	$(MAKE) -C ModernVulkanBlockGame

.PHONY: run
run: build
	./Build/Bin/ModernVulkanBlockGame
