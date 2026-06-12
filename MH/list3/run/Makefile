CXX      = clang++
CXXFLAGS = -O2 -std=c++17 -fopenmp
TARGET   = run
SRCS     = run.cpp utils.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)