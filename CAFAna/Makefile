# Extremely crude stop-gap makefile

CFLAGS := -std=c++17 -I.. -I${ROOT_INC} -g -Wall -Werror

# respect jenkins env vars
CFLAGS += $(shell if [[ x${QUALIFIER} == *debug* ]]; then echo ''; else echo '-O3'; fi )

LDFLAGS := -L${ROOTSYS}/lib -lCore

SRCS := $(wildcard Core/*.cxx)
HDRS := $(wildcard Core/*.h)
OBJS := $(patsubst %.cxx,tmp/%.o,$(SRCS))

all: prereqs ${OBJS} lib/libCAFAnaCoreExt.so

tmp/%.o: %.cxx ${HDRS}
	g++ $< ${CFLAGS} -c -fpic -o $@

lib/libCAFAnaCoreExt.so: ${OBJS}
	g++ -shared -o $@ tmp/Core/*.o ${LDFLAGS}

prereqs:
	@echo Checking all necessary env vars are set
	test ${ROOT_INC} # ROOT_INC
	mkdir -p tmp/Core
	mkdir -p lib

clean:
	rm -r tmp lib
