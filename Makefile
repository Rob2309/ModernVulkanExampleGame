
CONFIG:=Debug

.PHONY: build
build:
	$(MAKE) -C ModernVulkanBlockGame config=$(CONFIG)
	$(MAKE) -C Assets

.PHONY: run
run: build
	./Build/Bin/$(CONFIG)/ModernVulkanBlockGame

.PHONY: clean
clean: 
	$(MAKE) -C Assets clean
	rm -rf Build

.PHONY: build-debug
build-debug:
	$(MAKE) -j 4 build CONFIG=Debug

.PHONY: build-release
build-release:
	$(MAKE) -j 4 build CONFIG=Release

.PHONY: run-debug
run-debug:
	$(MAKE) -j 4 run CONFIG=Debug

.PHONY: run-release
run-release:
	$(MAKE) -j 4 run CONFIG=Release
