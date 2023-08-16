ifeq ($(OS), Windows_NT)
    LIBPATH := /mingw64/lib
    OSPRETTY := Windows
    BINPATH := /mingw64/bin
else
    ifeq ($(shell uname -s), Darwin)
        OSPRETTY := macOS
    else
        OSPRETTY := Linux
    endif
    LIBPATH := /usr/local/lib
    BINPATH := /usr/local/bin
endif
CXXVER := 14
OPTIM := 3
CXXFLAGS := -std=c++$(CXXVER) -O$(OPTIM) -Wall -Wextra -Wno-missing-braces -Wold-style-cast
ifeq ($(OSPRETTY), macOS)
    CXXFLAGS += -mmacosx-version-min=10.11 -Wunguarded-availability
else
    ifeq ($(OSPRETTY), Windows)
        CXXFLAGS += -Wno-deprecated-copy
    endif
endif
INCL := -I..
LIBS := ../libpazio.a
ifeq ($(OSPRETTY), macOS)
    LIBS += -framework Cocoa
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

REINSTALL := $(shell cmp -s paz-archive $(BINPATH)/paz-archive; echo $$?)

print-% : ; @echo $* = $($*)

all: $(EXE)

ifneq ($(REINSTALL), 0)
install: paz-archive
	cp paz-archive $(BINPATH)/
else
install: paz-archive
endif

%: %.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(INCL) $(LIBS)

clean:
	$(RM) $(EXE)
