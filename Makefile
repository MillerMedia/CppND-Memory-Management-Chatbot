clean:
	rm -rf ./build

build:
	mkdir build && cd build && cmake .. && make

cleanbuild:
	rm -rf ./build && mkdir build && cd build && cmake .. && make