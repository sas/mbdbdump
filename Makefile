CXX		?= g++
CXXFLAGS	:= -Wall -Wextra -Wno-deprecated-declarations -std=c++11
LDFLAGS		:= `pkg-config --libs openssl`

TARGET		:= mbdbdump
SOURCES		:= mbdbdump.cpp mbdb_record.cpp
OBJECTS		:= $(SOURCES:.cpp=.o)
DEPENDS		:= $(SOURCES:.cpp=.d)

VPATH		 = $(dir $(lastword $(MAKEFILE_LIST)))

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)

.PHONY: clean distclean

-include $(DEPENDS)
