CXXVER := 17
MINMACOSVER := 11.0

ifeq ($(OS), Windows_NT)
    OSPRETTY := Windows
    ifeq ($(MSYSTEM), UCRT64)
        CC := gcc
        CXX := g++
        LIBPATH := /ucrt64/lib
        BINPATH := /ucrt64/bin
    else ifeq ($(MSYSTEM), CLANG64)
        CC := clang
        CXX := clang++
        LIBPATH := /clang64/lib
        BINPATH := /clang64/bin
    else
        $(error Unsupported Windows environment)
    endif
else
    ifeq ($(shell uname -s), Darwin)
        OSPRETTY := macOS
        CC := clang
        CXX := clang++
    else
        OSPRETTY := Linux
        CC := gcc
        CXX := g++
    endif
    LIBPATH := /usr/local/lib
    BINPATH := /usr/local/bin
endif
CXXFLAGS := -std=c++$(CXXVER) -O3 -Wall -Wextra -Wno-missing-braces \
    -Wold-style-cast
ifeq ($(OSPRETTY), macOS)
    CXXFLAGS += -mmacosx-version-min=$(MINMACOSVER) -Wunguarded-availability
endif
ifeq ($(CC), clang)
    CXXFLAGS += -Wno-string-plus-int
endif
CXXFLAGS += -I..
LDLIBS := ../libpazio.a
ifeq ($(OSPRETTY), macOS)
    LDLIBS += -framework Cocoa
else ifeq ($(OSPRETTY), Windows)
    ifeq ($(CC), gcc)
        LDLIBS += -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
    else
        LDLIBS += -Wl,-Bstatic -lc++ -Wl,-Bdynamic
    endif
endif

SRC := $(wildcard *.cpp)
EXE := $(SRC:.cpp=)

print-% : ; @echo "$* = $($*)"

.PHONY: $(SRC)

all: $(EXE)

install: paz-archive
	@[[ -d $(BINPATH) ]] || \
	    { echo "mkdir -p $(BINPATH)"; mkdir -p $(BINPATH); }
	@cmp -s paz-archive $(BINPATH)/paz-archive && \
	    echo "Nothing to do for $(BINPATH)/paz-archive" || \
	    { echo "cp paz-archive $(BINPATH)/"; \
	    cp paz-archive $(BINPATH)/; }

ifeq ($(OSPRETTY), macOS)
%: %.cpp
	$(CXX) -arch arm64 -o $@_arm64 $< $(CXXFLAGS) $(INCL) $(LDFLAGS) \
	    $(LDLIBS)
	$(CXX) -arch x86_64 -o $@_x86_64 $< $(CXXFLAGS) $(INCL) $(LDFLAGS) \
	    $(LDLIBS)
	lipo -create -output $@ $@_arm64 $@_x86_64
endif

ifeq ($(OSPRETTY), macOS)
clean:
	$(RM) $(EXE) $(foreach n,$(EXE),$n_arm64 $n_x86_64)
else
clean:
	$(RM) $(EXE)
endif
