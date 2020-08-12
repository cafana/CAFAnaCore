# Simple wrapper for those of us who don't like having to cd to build, and
# never remember that it's make *install*

all:
	if [ ! -e build ]; then mkdir -p build; cd build; cmake -GNinja ..; fi
	ninja -C build -j`nproc --all` install

clean:
	if [ -e build ]; then ninja -C build clean; rm -r build; fi
