PROJNAME := PAZ_IO
LIBNAME := $(shell echo $(PROJNAME) | sed 's/_//g' | tr '[:upper:]' '[:lower:]')
ifeq ($(OS), Windows_NT)
    LIBPATH := /mingw64/lib
    INCLPATH := /mingw64/include
    OSPRETTY := Windows
else
    ifeq ($(shell uname -s), Darwin)
        OSPRETTY := macOS
    else
        OSPRETTY := Linux
    endif
    LIBPATH := /usr/local/lib
    INCLPATH := /usr/local/include
endif
CXXVER := 17
OPTIM := fast
ZIPNAME := $(PROJNAME)-$(OSPRETTY)
ifeq ($(OSPRETTY), Windows)
    ZIPCONTENTS := $(PROJNAME) lib$(LIBNAME).a util/paz-archive.exe
else
    ZIPCONTENTS := $(PROJNAME) lib$(LIBNAME).a util/paz-archive
endif
CFLAGS := -O$(OPTIM) -Wall -Wextra -Wno-missing-braces
ifeq ($(OSPRETTY), macOS)
    CFLAGS += -mmacosx-version-min=10.10
else
    ifeq ($(OSPRETTY), Windows)
        CFLAGS += -Wno-cast-function-type
    endif
endif
CXXFLAGS := -std=c++$(CXXVER) $(CFLAGS) -Wold-style-cast
ifeq ($(OSPRETTY), Windows)
    CXXFLAGS += -Wno-deprecated-copy
endif
ARFLAGS := -rcs

CSRC := $(wildcard *.c) $(wildcard *.cpp)
ifeq ($(OSPRETTY), macOS)
    MACOSEXCL := $(patsubst %_macos.mm, %.cpp, $(wildcard *_macos.mm))
    CSRC := $(filter-out $(MACOSEXCL), $(CSRC))
endif
OBJCSRC := $(wildcard *.mm)
OBJ := $(patsubst %.c, %.o, $(patsubst %.cpp, %.c, $(CSRC)))
ifeq ($(OSPRETTY), macOS)
    OBJ += $(OBJCSRC:%.mm=%.o)
endif

REINSTALLHEADER := $(shell cmp -s $(PROJNAME) $(INCLPATH)/$(PROJNAME); echo $$?)

print-% : ; @echo $* = $($*)

default: lib$(LIBNAME).a
	make -C test
	make -C util
	test/test

lib$(LIBNAME).a: $(OBJ)
	$(RM) lib$(LIBNAME).a
	ar $(ARFLAGS) lib$(LIBNAME).a $^

ifneq ($(REINSTALLHEADER), 0)
install: $(PROJNAME) lib$(LIBNAME).a
	cp $(PROJNAME) $(INCLPATH)/
	cp lib$(LIBNAME).a $(LIBPATH)/
	make -C util install
else
install: $(PROJNAME) lib$(LIBNAME).a
	cp lib$(LIBNAME).a $(LIBPATH)/
	make -C util install
endif

analyze: $(OBJCSRC)
	$(foreach n, $(OBJCSRC), clang++ --analyze $(n) $(CXXFLAGS) && $(RM) $(n:%.mm=%.plist);)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.mm
	$(CC) -c -o $@ $< $(CXXFLAGS)

clean:
	$(RM) $(OBJ) lib$(LIBNAME).a
	make -C test clean
	make -C util clean

zip: $(ZIPCONTENTS)
	zip -j $(ZIPNAME).zip $(ZIPCONTENTS)
