# CFLAGS is not defined by default
CFLAGS=-MMD -Wall -Wextra -Werror -Wno-format-overflow -std=c17 -march=x86-64 -fdiagnostics-color=always

SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=%.o) endian.o
DEP=$(OBJ:%.o=%.d)

EXE=fits

# add any additional libraries here ...
# $(addprefix -l, m pthread GL)
LIBS=$(addprefix -l,)

.PHONY: clean test

debug: CFLAGS += -g -Wno-unused-parameter -Wno-unused-variable
debug: $(EXE)

remake: clean debug
.NOTPARALLEL: remake

release: CFLAGS += -Os -s -Wno-unused-result -fno-ident -fno-asynchronous-unwind-tables -faggressive-loop-optimizations
release: clean $(EXE)
.NOTPARALLEL: release

.ONESHELL:
test: release
	@cd ~/Pictures/FITS/M64
	~/Projects/fits/fits -v -i jw03707-o100_t045_nircam_clear-f150w_i2d
.NOTPARALLEL: test

clean:
	$(RM) $(OBJ) $(DEP) $(EXE)

install: release
	sudo cp $(EXE) /usr/local/bin/$(EXE)

endian.o: endian.s
	as -o endian.o endian.s

$(EXE): $(OBJ)
	gcc -o $@ $^ $(LIBS)

-include $(DEP)
