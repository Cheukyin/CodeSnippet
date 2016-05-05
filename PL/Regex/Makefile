unittest: *.cpp *.hpp
	clang++ -std=c++11 -Wall -O0 -ggdb AST.cpp ASTDumpTest.cpp DerivativeTest.cpp EngineTest.cpp Parser.cpp ParserTest.cpp UnitTest.cpp CachedObjectTest.cpp -o unittest

.PHONY: test
test:
	./unittest
