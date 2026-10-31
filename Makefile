# Makefile for TIVIDY.
#   make           build ./tividy_gui (GUI) and ./tividy_demo (scripted demo)
#   make gui       build and start the graphical interface (opens your browser)
#   make demo      build and run the scripted demo
#   make clean     remove build files

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iincludes -Iapp -Iapp/gui
LDLIBS   =
ifeq ($(OS),Windows_NT)
  LDLIBS += -lws2_32
  EXE     = .exe
endif

ENGINE   = $(wildcard src/*.cpp)
INDUSTRY = app/Industries.cpp
GUI      = app/Json.cpp app/CustomWorkflow.cpp app/gui/HttpServer.cpp app/gui/GuiApi.cpp app/gui/gui_main.cpp

ENGINE_OBJ = $(ENGINE:%.cpp=build/%.o) $(INDUSTRY:%.cpp=build/%.o)
GUI_OBJ    = $(GUI:%.cpp=build/%.o)
DEMO_OBJ   = build/app/main.o
HEADERS    = $(wildcard includes/*.h) $(wildcard app/*.h) $(wildcard app/gui/*.h)

all: tividy_gui tividy_demo

tividy_gui: $(ENGINE_OBJ) $(GUI_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

tividy_demo: $(ENGINE_OBJ) $(DEMO_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp into build/ (rebuilds when a header changes)
build/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

gui: tividy_gui
	./tividy_gui$(EXE)

run: gui

demo: tividy_demo
	./tividy_demo$(EXE)

clean:
	rm -rf build tividy_gui tividy_demo

.PHONY: all gui run demo clean

# Automated unit and integration tests
TEST_SOURCES = $(wildcard tests/*Tests.cpp)
TEST_BINS = $(patsubst tests/%.cpp,build/tests/%,$(TEST_SOURCES))

build/tests/%: tests/%.cpp $(ENGINE_OBJ) build/app/Industries.o
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(ENGINE_OBJ) $(LDLIBS)

test: $(TEST_BINS)
	@set -e; for t in $(TEST_BINS); do \
		echo "Running $$t"; \
		./$$t; \
	done

.PHONY: test