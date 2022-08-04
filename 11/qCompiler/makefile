run: qCompiler.cpp \
     JackTokenizer.h JackTokenizer.cpp \
		 ParsePath.h ParsePath.cpp \
		 CompilationEngine.h CompilationEngine.cpp \
		 SymbolTable.h SymbolTable.cpp \
		 VMWriter.h VMWriter.cpp
	g++ qCompiler.cpp JackTokenizer.cpp ParsePath.cpp CompilationEngine.cpp SymbolTable.cpp VMWriter.cpp -o app

debug: qCompiler.cpp \
     JackTokenizer.h JackTokenizer.cpp \
		 ParsePath.h ParsePath.cpp \
		 CompilationEngine.h CompilationEngine.cpp \
		 SymbolTable.h SymbolTable.cpp \
		 VMWriter.h VMWriter.cpp
	g++ -g qCompiler.cpp JackTokenizer.cpp ParsePath.cpp CompilationEngine.cpp  SymbolTable.cpp VMWriter.cpp -o app