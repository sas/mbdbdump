SOURCES		:= src/mbdbdump.cpp src/mbdb_record.cpp
DOCSOURCES	:= doc/mbdbdump.1.ronn
PREFIX		:= /usr/local

CXX		?= g++
CXXFLAGS	:= `pkg-config --cflags openssl` -std=c++11 -Wall -Wextra -Wno-deprecated-declarations
LDFLAGS		:= `pkg-config --libs openssl`

TARGET		:= mbdbdump
OBJECTS		:= $(SOURCES:.cpp=.o)
DEPENDS		:= $(SOURCES:.cpp=.d)
DOCTARGETS	:= $(DOCSOURCES:.1.ronn=.1)

VPATH		:= $(dir $(lastword $(MAKEFILE_LIST)))

all: $(TARGET)

install: all $(DOCTARGETS)
	install -d $(PREFIX)/bin/
	install -d $(PREFIX)/share/man/
	install -s -t $(PREFIX)/bin/ $(TARGET)
	install -t $(PREFIX)/share/man/ $(DOCTARGETS)

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)
	rm -f $(DOCTARGETS)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<

%.1: %.1.ronn
	@mkdir -p $(dir $@)
	ronn --roff --pipe $< >$@

.PHONY: all install clean distclean

-include $(DEPENDS)
