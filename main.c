#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "gurobi_c.h"
/*#pragma warning(disable : 4996)*/

#include "parser.h"
#include "commands_imp.h"
#include "doubly_linked_list.h"
#include "user_commands.h"
#include "ilp.h"

/*num = (rand() % (upper � lower + 1)) + lower*/

int main() {
	
	initialDDL();
	 initial();
	return 0;
	
}

