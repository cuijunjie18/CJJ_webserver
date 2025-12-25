PROJECT_SOURCE_DIR ?= $(abspath ./)
BUILD_DIR ?= $(PROJECT_SOURCE_DIR)/build
EXECUTABLE_OUTPUT_PATH ?= ${BUILD_DIR}/bin

ENABLE_UT_TEST ?= OFF

CMAKE_ARGS ?= \
	-DENABLE_UT_TEST=$(ENABLE_UT_TEST) \
	-DEXECUTABLE_OUTPUT_PATH=$(EXECUTABLE_OUTPUT_PATH) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build:
	cmake -B build $(CMAKE_ARGS)
	cmake --build build -j

webbench:
	cd WebBench && $(MAKE) webbench

clean:
	rm -rf build
