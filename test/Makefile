ifeq ($(OS), Windows_NT)
    LIBPATH := /mingw64/lib
    OSPRETTY := Windows
else
    ifeq ($(shell uname -s), Darwin)
        OSPRETTY := macOS
    else
        OSPRETTY := Linux
    endif
    LIBPATH := /usr/local/lib
endif
CXXVER := 17
CXXFLAGS := -std=c++$(CXXVER) -O3 -Wall -Wextra -Wno-missing-braces -Wold-style-cast
ifeq ($(OSPRETTY), macOS)
    CXXFLAGS += -mmacosx-version-min=10.11 -Wunguarded-availability -Wno-string-plus-int
else
    ifeq ($(OSPRETTY), Windows)
        CXXFLAGS += -Wno-deprecated-copy -static
    endif
endif
CXXFLAGS += -I..
LDLIBS := ../libpazio.a
ifeq ($(OSPRETTY), macOS)
    LDLIBS += -framework Cocoa
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

.PHONY: $(SRC)

all: $(EXE)

clean:
	$(RM) $(EXE)

print-% : ; @echo $* = $($*)
