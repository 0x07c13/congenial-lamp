CXX = g++
DEBUG = 1
# CXXFLAGS = -Wall -Wextra -std=c++11 -O3 -g -I./include
CXXFLAGS = -Wall -Wextra -std=c++11 -I./include

ifeq ($(DEBUG), 1) 
	CXXFLAGS += -g
else
	CXXFLAGS += -O3
endif

LDFLAGS = -lpthread -lmysqlclient

SRCDIR = src
OBJDIR = build
BINDIR = bin

SOURCES := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES)) 
EXECUTABLE := $(BINDIR)/congenial-lamp

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

