ifeq ($(OS), Windows_NT)
    LIBPATH := /mingw64/lib
    OSPRETTY := Windows
else
    ifeq ($(shell uname -s), Darwin)
        LIBPATH := /usr/local/lib
        OSPRETTY := macOS
    else
        LIBPATH := /usr/local/lib64
        OSPRETTY := Linux
    endif
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
else
    LIBS += $(LIBPATH)/libglfw3.a -lstdc++fs
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

print-% : ; @echo $* = $($*)

all: $(EXE)

%: %.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(INCL) $(LIBS)

clean:
	$(RM) $(EXE)
