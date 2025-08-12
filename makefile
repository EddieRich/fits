#==============================================================================
#
# Makefile starter
#
# Automatic Variables
#   $@ The file name of the target of the rule
#   $< The name of the first prerequisite
#   $? The names of all the prerequisites that are newer than the target
#   $^ The names of all the prerequisites
#
#==============================================================================
#
# Based on
# https://www.reddit.com/r/C_Programming/comments/1ghy847/the_perfect_makefile/
#
# also see ...
# https://www.gnu.org/software/make/manual/html_node/index.html
#
# modified flags, added .PHONY
# add OS dependant commands
#

#$(CC) defaults to cc, which defaults to gcc for *.c and g++ for *.cpp

# CFLAGS is not defined by default
CXXFLAGS=-MMD -Wall -Wextra -Werror -std=c++17 -march=x86-64 -fdiagnostics-color=always

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)

# linux does not define the OS environment variable
# if in the future it does, it won't be Windows_NT
ifeq ($(OS),Windows_NT)
	EXE = main.exe
	RM = del /Q
else
	EXE = main
#RM is predefined as RM = rm -f
endif

# add any additional libraries here ...
# $(addprefix -l, m pthread GL)
LIBS=$(addprefix -l,)

.PHONY: clean #install

debug: CFLAGS += -g
debug: $(EXE)

remake: clean debug
.NOTPARALLEL: remake

release: CXXFLAGS += -Os -s -fno-ident -fno-asynchronous-unwind-tables -faggressive-loop-optimizations
release: clean $(EXE)
.NOTPARALLEL: release

clean:
	$(RM) $(OBJ) $(DEP) $(EXE)

#install: release
#	cp $(EXE) $(TARGET)/bin

$(EXE): $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

-include $(DEP)

# the default .o:.c implicit rule is ...
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c
# the -c flag implies generating a .o file.
#
# only define this if there are extra steps or drastic changes
# in most cases everything is handled by the flags
#
#%.o: %.c
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

#==============================================================================
# most are blank
# CFLAGS was set at the top of this file (otherwise blank as well)
info :
	@echo Assembler $(AS)
	@echo Assembler Flags $(ASFLAGS)
	@echo Preprocessor $(CPP)
	@echo Preprocessor Flags $(CPPFLAGS)
	@echo C Compiler $(CC)
	@echo C Compiler Flags $(CFLAGS)
	@echo C++ Compiler $(CXX)
	@echo C++ Compiler Flags $(CXXFLAGS)
	@echo Linker (ld) Flags $(LDFLAGS)
	@echo Linker (ld) Libs $(LDLIBS)
