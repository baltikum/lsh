#include <stdio.h>
#include <string.h>
#include <stdlib.h> // getenv()

#include "parse.h"



#include <dirent.h>

const char* extractpath(Command *cmd, _Bool* isavailable) {

	const char* paths = getenv("PATH");
	char copiedpath[strlen(paths)];
	strcpy(copiedpath,paths); // Kopierar paths då den är immutable

	char* spaths[30];
	char* pointer; // Pekare för att stega igenom copiedpath
	pointer = strtok(copiedpath,":"); // Stegar fram till :

	int i = 0;
	while( pointer != NULL ) {
		spaths[i] = pointer;
		++i;
		pointer = strtok(NULL,":"); // Stega framåt till :
	}
	
	DIR *dir;
	char* commandtosearchfor = *cmd->pgm->pgmlist;
	long commandlength = strlen(commandtosearchfor);


	for ( int j = 0; j < i; j++ ) { // Använder förra i då den har koll på mängden paths

		const char* retrievedpath = spaths[j];
		dir = opendir(retrievedpath);

		if ( dir == NULL ) {

		} else {
			
			struct dirent *dirpointer; // directory entry struct

			while( dirpointer = readdir(dir)) { // Loop läser in entrys ifrån directory

				char* pointer = dirpointer->d_name;
				char* pointer2 = commandtosearchfor;

				if ( strcmp(pointer,pointer2) == 0 ) {
					return pointer;	
				};
			};
			closedir(dir);// Stäng öppnat directory
		};
	};
};
