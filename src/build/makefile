#Original starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
all: sjc

debug: CFLAGS += -g
debug: sjc

CC = gcc $(CFLAGS)

sjc: sjc.o lex.yy.o errors.o parser.o AST.o ASTPrintTree.o offset_ref.o label_ref.o type.o environment.o hash.o semantic.o AAT.o AATBuildTree.o AATPrintTree.o register.o label.o codegenArm64.o env_arm64.o
	$(CC) -o sjc sjc.o lex.yy.o errors.o parser.o AST.o ASTPrintTree.o label_ref.o offset_ref.o type.o environment.o hash.o semantic.o AAT.o AATBuildTree.o AATPrintTree.o registerArm64.o label.o codegenArm64.o env_arm64.o

codegenArm64.o: ../assembly/AAT.h ../codegen/registerArm64.h ../assembly/label.h ../codegen/codegenArm64.c
	$(CC) -c ../codegen/codegenArm64.c

semantic.o: ../parser/AST.h ../assembly/AAT.h ../assembly/AATBuildTree.h ../semantic/semantic.h ../semantic/semantic.c ../semantic/type.h ../semantic/environment1.h ../semantic/environment2.h ../codegen/registerArm64.h ../assembly/label.h
	$(CC) -c ../semantic/semantic.c

label.o: ../assembly/label.h ../assembly/label.c
	$(CC) -c ../assembly/label.c

register.o: ../codegen/registerArm64.h ../codegen/registerArm64.c
	$(CC) -c ../codegen/registerArm64.c

AST.o: ../parser/AST.h ../parser/AST.c
	$(CC) -c ../parser/AST.c

AAT.o: ../assembly/AAT.h ../assembly/AAT.c
	$(CC) -c ../assembly/AAT.c

AATBuildTree.o: ../assembly/AATBuildTree.h ../assembly/AATBuildTree.c
	$(CC) -c ../assembly/AATBuildTree.c

hash.o: ../semantic/hash.h ../semantic/hash.c
	$(CC) -c ../semantic/hash.c

offset_ref.o: ../lib/offset_ref.h ../lib/offset_ref.c
	$(CC) -c ../lib/offset_ref.c

label_ref.o: ../lib/label_ref.h ../lib/label_ref.c
	$(CC) -c ../lib/label_ref.c

environment.o: ../semantic/environment.c ../semantic/environment1.h ../semantic/environment2.h
	$(CC) -c ../semantic/environment.c

env_arm64.o: ../semantic/env_arm64.c ../semantic/env_arm64.h
	$(CC) -c ../semantic/env_arm64.c

type.o: ../semantic/type.h ../semantic/environment1.h ../semantic/type.c
	$(CC) -c ../semantic/type.c

AATPrintTree.o: ../assembly/AATPrintTree.h ../assembly/AATPrintTree.c
	$(CC) -c ../assembly/AATPrintTree.c

ASTPrintTree.o: ../parser/ASTPrintTree.c ../parser/ASTPrintTree.h ../parser/AST.h
	$(CC) -c ../parser/ASTPrintTree.c

errors.o: ../errors/errors.c ../errors/errors.h
	$(CC) -c ../errors/errors.c

sjc.o: ../codegen/sjc.c ../parser/AST.h ../assembly/AAT.h ../parser/ASTPrintTree.h ../errors/errors.h ../semantic/semantic.h
	$(CC) -c ../codegen/sjc.c

parser.o: parser.c parser.h
	$(CC) -c parser.c

lex.yy.o: lex.yy.c parser.h
	$(CC) -c lex.yy.c

parser.h: ../parser/sjava.yacc
	bison --header=parser.h --output=parser.c ../parser/sjava.yacc

parser.c: ../parser/sjava.yacc
	bison --header=parser.h --output=parser.c ../parser/sjava.yacc

lex.yy.c: ../lexer/sjava.lex parser.h
	flex --header-file=lex.yy.h ../lexer/sjava.lex

clean:
	rm -f sjc semantic.o lex.yy.o lex.yy.c lex.yy.h errors.o parser.o AST.o ASTPrintTree.o core parser.o parser.h parser.c y.output AST.o type.o environment.o semantictest.o hash.o label.o sjc.o AATPrintTree.o AATBuildTree.o codegenArm64.o AAT.o registerArm64.o env_arm64.o offset_ref.o label_ref.o
