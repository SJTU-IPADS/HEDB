SRCS = $(wildcard *.c)
EXES = $(SRCS:%.c=%)
CFLAGS = -Wall -Werror

all: $(EXES)
	sudo chown root:root $(EXES)
	sudo chmod u+s $(EXES)

clean:
	find . -type f -executable -print | xargs rm
