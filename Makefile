CC=clang
# CFLAGS=-g -Wall -Isrc -rdynamic -DNDEBUG $(OPTFLAGS) 
CFLAGS=-g -Wall -Isrc -DNDEBUG $(OPTFLAGS) 
# -Isrc - включить все header-files из src
# -rdynamic - сказать линкеру включить все символы, не только используемые
# 	это нужно для bactrace или dlopen
LIBS=-ldl $(OPTLIBS)
# options used when linking libraries
PREFIX?=/usr/local
# optional variable (?=) used only if PREFIX not already set up

SOURCES=$(wildcard src/**/*.c src/*.c)
# searches for .c files in src and in src subdirectories
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
# patsubst заменяет все .c названия на .o

TEST_SRC=$(wildcard tests/*_tests.c)
# searches for ._tests.c files in tests
TESTS=$(patsubst %.c,%,$(TEST_SRC))
# удаляет все .c суффиксы

TARGET=build/liblcthw.a
# заменить YOUR_LIBRARY на название программы
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))
# то же что TARGET только с .so

OS=$(shell lsb_release -si)
ifeq ($(OS),Ubuntu)
	LDLIBS=-llcthw -lbsd -L./build -lm
endif

# The Target Build
all: $(TARGET) $(SO_TARGET) tests
# all: will run as a first option if no options provided to make

dev: CFLAGS=-g -Wall -Isrc -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
# fPIC == generate position-independent code (it nessesary for libraries)
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	# ar makes the TARGET
	# syntax $@ $(OBJECTS) is a way of saying,
	# "put the target for this Makefile source here and all the OBJECTS after that."
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: LDLIBS += $(TARGET)
tests: CFLAGS=-g -Wall -Isrc -Wextra $(OPTFLAGS)
# trick for modifying the CFLAGS variable to add the TARGET to the build
# so that each of the test programs will be linked
# with the TARGET library.
# In this case, it will add build/libYOUR_LIBRARY.a to the linking.
tests: $(TESTS) tests/minunit.h
	# $(CC) $(CFLAGS) tests/runtests.c -o tests/runtests
	# tests/runtests
	sh tests/runtests.sh

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*dSYM" -print`
	# dSYM - какие-то дебаг файлы

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@grep -E '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|strpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
	# || true at the end is a way to prevent make
	# from thinking that egrep failed if it doesn't find errors
