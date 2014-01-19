TARGET		:= mbdbdump
SOURCES		:= src/mbdbdump.cpp src/mbdb_record.cpp
PREFIX		:= /usr/local

CXX		?= g++
CXXFLAGS	:= `pkg-config --cflags openssl` -std=c++11 -Wall -Wextra -Wno-deprecated-declarations
LDFLAGS		:= `pkg-config --libs openssl`

OBJECTS		:= $(SOURCES:.cpp=.o)
DEPENDS		:= $(SOURCES:.cpp=.d)

VPATH		:= $(dir $(lastword $(MAKEFILE_LIST)))

all: $(TARGET)

install: all
	install -d $(PREFIX)/bin/
	install -s -t $(PREFIX)/bin/ $(TARGET)

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<

.PHONY: all install clean distclean

-include $(DEPENDS)
