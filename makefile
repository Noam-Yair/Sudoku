GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56


sudoku-console: main.o print.o commands_imp.o parser.o doubly_linked_list.o ilp.o stack.o user_commands.o 
	gcc main.c parser.c print.c commands_imp.c doubly_linked_list.c ilp.c stack.c user_commands.c $(GUROBI_LIB) -o sudoku-console

all: main.o parser.o print.o commands_imp.o doubly_linked_list.o ilp.o stack.o user_commands.o
	gcc main.c parser.c print.c commands_imp.c doubly_linked_list.c ilp.c stack.c user_commands.c $(GUROBI_LIB) -o sudoku-console

main.o: main.c parser.h print.h commands_imp.h doubly_linked_list.h ilp.h 
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors  $(GUROBI_COMP) -c main.c

parser.o: parser.c commands_imp.h doubly_linked_list.h ilp.h parser.h  
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c parser.c

print.o: print.c print.h commands_imp.h
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c print.c

commands_imp.o: commands_imp.c commands_imp.h doubly_linked_list.h ilp.h stack.h
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c commands_imp.c

doubly_linked_list.o: doubly_linked_list.c doubly_linked_list.h commands_imp.h parser.h 
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c doubly_linked_list.c

ilp.o: ilp.c ilp.h commands_imp.h parser.h 
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c ilp.c

stack.o: stack.c stack.h parser.h print.h commands_imp.h
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c stack.c

user_commands.o: user_commands.c user_commands.h parser.h commands_imp.h doubly_linked_list.h stack.h print.h
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c user_commands.c

clean:
	rm -f *.o sudoku-console
