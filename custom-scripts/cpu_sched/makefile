BUILDROOT_DIR := ../..
KDIR := $(BUILDROOT_DIR)/output/build/linux-custom
COMPILER := $(BUILDROOT_DIR)/output/host/bin/i686-buildroot-linux-gnu-gcc

all:

	$(COMPILER) -o sched_profiler sched_profiler.c -lpthread -Wall
	cp sched_profiler $(BUILDROOT_DIR)/output/target/bin

clean:
	rm sched_profiler deadline
