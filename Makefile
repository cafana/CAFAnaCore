# Simple wrapper for those of us who don't like having to cd to build, and
# never remember that it's make *install*


# Defaults
CMAKE_BUILD_TYPE ?= Release
CMAKE_CXX_COMPILER ?= g++
CAFANACORE_USE_STAN ?= On
USE_STAN_THREADS ?= Off

all:
	if [ ! -e build ]; then mkdir -p build; cd build; cmake -GNinja .. -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) -DCMAKE_CXX_COMPILER=$(CMAKE_CXX_COMPILER) -DCAFANACORE_USE_STAN=$(CAFANACORE_USE_STAN) -DUSE_STAN_THREADS=$(USE_STAN_THREADS); fi
	ninja -C build -j`nproc --all` install

clean:
	if [ -e build ]; then ninja -C build clean; rm -r build; fi
