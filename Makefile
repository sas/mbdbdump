CC	:= clang++
CFLAGS	:= -Wall -Wextra -Wno-deprecated-declarations -std=c++11
LDFLAGS	:= `pkg-config --libs openssl`

TARGET	:= mbdbdump
SOURCES	:= mbdbdump.cpp mbdb_record.cpp
OBJECTS	:= $(SOURCES:.cpp=.o)
DEPENDS	:= $(SOURCES:.cpp=.d)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)

.PHONY: all clean distclean

-include $(DEPENDS)
