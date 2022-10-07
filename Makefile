CPPFLAGS += -std=c++17 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include
CPPCLASSES = include/ast/*.cpp include/program/*.cpp include/utils/*.cpp include/scope/*.cpp

bin/c_compiler : bin/compiler src/wrapper.sh
	cp src/wrapper.sh bin/c_compiler
	chmod u+x bin/c_compiler

bin/compiler : src/compiler.cpp src/parser.tab.cpp src/parser.tab.hpp src/lexer.yy.cpp
	mkdir -p bin
	g++ $(CPPFLAGS) ${CPPCLASSES} -o bin/compiler $^

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y include/inc.hpp include/ast/* include/program/* include/utils/* include/scope/*
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.l src/parser.tab.hpp include/inc.hpp include/ast/* include/program/* include/utils/* include/scope/*
	flex -o src/lexer.yy.cpp src/lexer.l

clean :
	rm -f src/*.o
	rm -f bin/*
	rm -f src/*.tab.*pp
	rm -f src/*.yy.cpp
	rm -f src/parser.output
