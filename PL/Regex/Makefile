unittest: *.cpp *.hpp
	clang++ -std=c++11 -Wall -O0 -g3 -ggdb AST.cpp ASTDumpTest.cpp DerivativeTest.cpp DerivativeEngineTest.cpp Parser.cpp ParserTest.cpp UnitTest.cpp CachedObjectTest.cpp NFATest.cpp -o unittest

.PHONY: test
test:
	./unittest
