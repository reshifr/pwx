DIST_DIR = dist

.PHONY: build
build:
	rm -rf winhello/build
	(cd winhello; rm -rf build; cmake -B build; cmake --build build --config Release)
	mkdir -p $(DIST_DIR)
	cp winhello/build/Release/winhello.dll $(DIST_DIR)/
	go build -o $(DIST_DIR)/
