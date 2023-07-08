.PHONY: all build run test clean install

all: build

build: 
	cmake -B build -S ./src -DTEE_TYPE=CVM # -DENABLE_PARALLEL=ON
	cmake --build build

BUILDDIR := build
TYPES := int text timestamp
SOURCES :=  ${addsuffix _ops.c, ${addprefix plain_, ${TYPES}}}
SOURCE_DIR := src/privacy_zone/plain_ops

BC_OBJECTS := ${addprefix ${BUILDDIR}/, $(SOURCES:%.c=%.bc)} 

CLANG_FLAGS := -I src/include -I src/privacy_zone/include -I src/privacy_zone -emit-llvm -c -g

${BUILDDIR}/%.bc: ${SOURCE_DIR}/%.c
	mkdir -p build
	@echo from file $@ $<
	clang ${CLANG_FLAGS} $< -o $@

klee_wrapper: ${BC_OBJECTS}
	clang ${CLANG_FLAGS} src/privacy_zone/like_match.c -o ${BUILDDIR}/like_match.bc
	clang ${CLANG_FLAGS} src/privacy_zone/klee_wrapper/wrapper.c -o ${BUILDDIR}/wrapper.bc
	clang ${CLANG_FLAGS} -I src/privacy_zone/klee_wrapper src/privacy_zone/klee_wrapper/prefix_udf.c -o ${BUILDDIR}/prefix_udf.bc
	llvm-link ${BUILDDIR}/wrapper.bc ${BUILDDIR}/like_match.bc ${BC_OBJECTS} ${BUILDDIR}/prefix_udf.bc  -o ${BUILDDIR}/whole.bc

install: 
	sudo cmake --install build

BENCHMARK_DIR=benchmark

run:
	fallocate -l 16M /dev/shm/ivshmem
	chmod a+rwx /dev/shm/ivshmem
	./build/ops_server >> /dev/null &

stop:
	pkill ops_server

test:
	echo "test not implemented"

clean:
	rm -rf build
