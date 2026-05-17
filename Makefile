CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
LDFLAGS = - lreadline
TARGET = see-shell
SRC = src/shell.cpp src/main.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod 755 /usr/local/bin/$(TARGET)
	@echo "Installed to /usr/local/bin/$(TARGET)"

.PHONY: all clean install
