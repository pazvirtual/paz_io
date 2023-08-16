CXXVER := 17
MINMACOSVER := 10.11

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
CXXFLAGS := -std=c++$(CXXVER) -O3 -Wall -Wextra -Wno-missing-braces -Wold-style-cast
ifeq ($(OSPRETTY), macOS)
    CXXFLAGS += -mmacosx-version-min=$(MINMACOSVER) -Wunguarded-availability -Wno-string-plus-int
else
    ifeq ($(OSPRETTY), Windows)
        CXXFLAGS += -Wno-deprecated-copy -static
    endif
endif
CXXFLAGS += -I..
LDLIBS := ../libpazio.a
ifeq ($(OSPRETTY), macOS)
    LDLIBS += -framework Cocoa
else
    ifeq ($(OSPRETTY), Windows)
        LDFLAGS += -static-libgcc -static-libstdc++
    endif
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

print-% : ; @echo $* = $($*)

all: $(EXE)

install: paz-archive
	cmp -s paz-archive $(BINPATH)/paz-archive || cp paz-archive $(BINPATH)/

ifeq ($(OSPRETTY), macOS)
%: %.cpp
	$(CXX) -arch arm64 -o $@_arm64 $< $(CXXFLAGS) $(INCL) $(LDFLAGS) $(LDLIBS)
	$(CXX) -arch x86_64 -o $@_x86_64 $< $(CXXFLAGS) $(INCL) $(LDFLAGS) $(LDLIBS)
	lipo -create -output $@ $@_arm64 $@_x86_64
endif

clean:
	$(RM) $(EXE)
