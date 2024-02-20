CXX = g++
CFLAGS = -Wall -Wextra -std=c++11 -O3 -g -I./include
LDFLAGS = -pthread -lmysqlclient

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXECUTABLE := $(BINDIR)/congenial-lamp

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

