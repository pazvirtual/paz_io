PROJNAME := PAZ_Engine
ifeq ($(OS), Windows_NT)
    LIBPATH := /mingw64/lib
    EIGENPATH := /mingw64/include
    OSPRETTY := Windows
else
    ifeq ($(shell uname -s), Darwin)
        LIBPATH := /usr/local/lib
        EIGENPATH := /opt/local/include
        OSPRETTY := macOS
    else
        LIBPATH := /usr/local/lib64
        EIGENPATH := /usr/local/include
        OSPRETTY := Linux
    endif
endif
CXXVER := 14
OPTIM := fast
EIGENPATH := $(EIGENPATH)/eigen3/Eigen
CXXFLAGS := -std=c++$(CXXVER) -O$(OPTIM) -Wall -Wextra -Wno-missing-braces
ifeq ($(OSPRETTY), macOS)
    CXXFLAGS += -mmacosx-version-min=10.10
else
    ifeq ($(OSPRETTY), Windows)
        CXXFLAGS += -Wno-deprecated-copy
    endif
    CXXFLAGS += -pthread
endif
INCL := -I.. -I$(EIGENPATH)
LIBS := ../libpazio.a
ifeq ($(OSPRETTY), macOS)
    LIBS += -framework QuartzCore -framework MetalKit -framework Metal -framework Cocoa
else
    LIBS += $(LIBPATH)/libglfw3.a -lstdc++fs
    ifeq ($(OSPRETTY), Linux)
        LIBS += -lGL -lX11 -ldl
    else
        LIBS += -lgdi32 -lopengl32
    endif
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

print-% : ; @echo $* = $($*)

.PHONY: $(SRC)

all: $(EXE)

%: %.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(INCL) $(LIBS)

clean:
	$(RM) $(EXE)
