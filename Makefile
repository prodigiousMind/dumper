CXX = g++
CXXFLAGS = -std=c++11

dumper: ./_source/dumper.cpp ./_source/dumperfunc.cpp ./_headers/headerDUMP.h
	$(CXX) $(CXXFLAGS) ./_source/dumper.cpp ./_source/dumperfunc.cpp -o dumper
