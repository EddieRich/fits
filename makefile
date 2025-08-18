# CFLAGS is not defined by default
CFLAGS=-MMD -Wall -Wextra -Werror -std=c17 -march=x86-64 -fdiagnostics-color=always

SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=%.o)
DEP=$(OBJ:%.o=%.d)

EXE=fits

# add any additional libraries here ...
# $(addprefix -l, m pthread GL)
LIBS=$(addprefix -l,)

.PHONY: clean #install

debug: CFLAGS += -g -Wno-unused-parameter -Wno-unused-variable
debug: $(EXE)

remake: clean debug
.NOTPARALLEL: remake

release: CFLAGS += -Os -s -fno-ident -fno-asynchronous-unwind-tables -faggressive-loop-optimizations
release: clean $(EXE)
.NOTPARALLEL: release

clean:
	$(RM) $(OBJ) $(DEP) $(EXE)

#install: release
#	cp $(EXE) $(TARGET)/bin

$(EXE): $(OBJ)
	gcc -o $@ $^ $(LIBS)

-include $(DEP)
