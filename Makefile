# Simple wrapper for those of us who don't like having to cd to build, and
# never remember that it's make *install*

all:
	if [ x$(CMAKE_BUILD_TYPE) == x ]; then printf "\n\n\nCMAKE_BUILD_TYPE not set. Defaulting to 'Release'\n\n\n"; CMAKE_BUILD_TYPE=Release; fi
	if [ ! -e build ]; then mkdir -p build; cd build; cmake -GNinja .. -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE); fi
	ninja -C build -j`nproc --all` install

clean:
	if [ -e build ]; then ninja -C build clean; rm -r build; fi
