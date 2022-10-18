#Original starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/

sjc: sjc.o lex.yy.o errors.o parser.o AST.o ASTPrintTree.o type.o environment.o hash.o semantic.o AAT.o AATBuildTree.o AATPrintTree.o register.o label.o codegenArm64.o env_arm64.o
	gcc -o sjc sjc.o lex.yy.o errors.o parser.o AST.o ASTPrintTree.o type.o environment.o hash.o semantic.o AAT.o AATBuildTree.o AATPrintTree.o registerArm64.o label.o codegenArm64.o env_arm64.o

codegenArm64.o: ../assembly/AAT.h ../codegen/registerArm64.h ../assembly/label.h ../codegen/codegenArm64.c
	gcc -c ../codegen/codegenArm64.c

semantic.o: ../parser/AST.h ../assembly/AAT.h ../assembly/AATBuildTree.h ../semantic/semantic.h ../semantic/semantic.c ../semantic/type.h ../semantic/environment1.h ../semantic/environment2.h ../codegen/registerArm64.h ../assembly/label.h
	gcc -c ../semantic/semantic.c

label.o: ../assembly/label.h ../assembly/label.c
	gcc -c ../assembly/label.c

register.o: ../codegen/registerArm64.h ../codegen/registerArm64.c
	gcc -c ../codegen/registerArm64.c

AST.o: ../parser/AST.h ../parser/AST.c
	gcc -c ../parser/AST.c

AAT.o: ../assembly/AAT.h ../assembly/AAT.c
	gcc -c ../assembly/AAT.c

AATBuildTree.o: ../assembly/AATBuildTree.h ../assembly/AATBuildTree.c
	gcc -c ../assembly/AATBuildTree.c

hash.o: ../semantic/hash.h ../semantic/hash.c
	gcc -c ../semantic/hash.c

environment.o: ../semantic/environment.c ../semantic/environment1.h ../semantic/environment2.h
	gcc -c ../semantic/environment.c

env_arm64.o: ../semantic/env_arm64.c ../semantic/env_arm64.h
	gcc -c ../semantic/env_arm64.c

type.o: ../semantic/type.h ../semantic/environment1.h ../semantic/type.c
	gcc -c ../semantic/type.c

AATPrintTree.o: ../assembly/AATPrintTree.h ../assembly/AATPrintTree.c
	gcc -c ../assembly/AATPrintTree.c

ASTPrintTree.o: ../parser/ASTPrintTree.c ../parser/ASTPrintTree.h ../parser/AST.h
	gcc -c ../parser/ASTPrintTree.c

errors.o: ../errors/errors.c ../errors/errors.h
	gcc -c ../errors/errors.c

sjc.o: ../codegen/sjc.c ../parser/AST.h ../assembly/AAT.h ../parser/ASTPrintTree.h ../errors/errors.h ../semantic/semantic.h
	gcc -c ../codegen/sjc.c

parser.o: parser.c parser.h
	gcc -c parser.c

lex.yy.o: lex.yy.c parser.h
	gcc -c lex.yy.c

parser.h: ../parser/sjava.yacc
	bison --header=parser.h --output=parser.c ../parser/sjava.yacc

parser.c: ../parser/sjava.yacc
	bison --header=parser.h --output=parser.c ../parser/sjava.yacc

lex.yy.c: ../lexer/sjava.lex parser.h
	flex --header-file=lex.yy.h ../lexer/sjava.lex

clean:
	rm -f sjc semantic.o lex.yy.o lex.yy.c lex.yy.h errors.o parser.o AST.o ASTPrintTree.o core parser.o parser.h parser.c y.output AST.o type.o environment.o semantictest.o hash.o label.o sjc.o AATPrintTree.o AATBuildTree.o codegenArm64.o AAT.o registerArm64.o env_arm64.o
