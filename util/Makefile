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
OPTIM := fast
CXXFLAGS := -std=c++$(CXXVER) -O$(OPTIM) -Wall -Wextra -Wno-missing-braces
ifeq ($(OSPRETTY), macOS)
    CXXFLAGS += -mmacosx-version-min=10.10
else
    ifeq ($(OSPRETTY), Windows)
        CXXFLAGS += -Wno-deprecated-copy
    endif
    CXXFLAGS += -pthread
endif
INCL := -I..
LIBS := ../libpazio.a
ifeq ($(OSPRETTY), macOS)
    LIBS += -framework Cocoa
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

print-% : ; @echo $* = $($*)

all: $(EXE)

install: paz-archive
	cp paz-archive $(BINPATH)/

%: %.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(INCL) $(LIBS)

clean:
	$(RM) $(EXE)
