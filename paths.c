#include <stdio.h>
#include <string.h>
#include <stdlib.h> // getenv()

#include "parse.h"



#include <dirent.h>

const char* extractpath(Command *cmd, _Bool* isavailable) {

	const char* paths = getenv("PATH"); // Hämtar miljövariabeln
	//printf("PATH: %s\n",paths);

	long pathlength = strlen(paths); // Kolla längden av denna
	//printf("PATHLENGTH: %ld\n",pathlength);

	char copiedpath[pathlength];
	strcpy(copiedpath,paths); // Kopierar den då den är immutable
	//printf("PATH COPIED: %s\n",copiedpath);

	int numberofpaths = 1;
	int pathsize = 0;
	int largestpathsize = 0;
	for ( int i = 0; i<pathlength ; i++) {
		if( copiedpath[i] == ':') {
			numberofpaths++;
			pathsize = 0;
		} else {
			pathsize++;
		}

		if ( pathsize > largestpathsize) {
			largestpathsize = pathsize;
		}
	}


	char * spaths[15];
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


	for ( int j = 0; j < i; j++ ) {

		const char* retrievedpath = spaths[j];

		dir = opendir(retrievedpath);

		if ( dir == NULL ) {
			printf("Couldnt open path directory.\n");
		} else {
			
			struct dirent *dirpointer; // directory entry struct

			while( dirpointer = readdir(dir)) { // Loop läser in entrys ifrån directory

				char* pointer = dirpointer->d_name;
				char* pointer2 = commandtosearchfor;

				//printf("Program: %s  ",pointer);

				long pointerlength = strlen(pointer);

				if ( commandlength == pointerlength) { // Fortsätt om de är lika långa
					long k = 0;

					while ( *pointer == *pointer2 ) { // Kolla char för char, k<command för att pekarna
						k++;
						//printf("%s should be %s \n ",pointer,pointer2);
						
						pointer++;
						pointer2++;

						if ( k == (commandlength) ) { // +1 ??? varför ?Kollar om det är matchning
							*isavailable = 1;
							printf("Command found in %s\n",retrievedpath);

							char* temp = malloc(strlen(retrievedpath));
							//location = malloc(strlen(retrievedpath) + 1);

							strcpy(temp,retrievedpath);

							//printf("temp in extractfunc is now = %s\n",temp);
							//printf("location in extractfunc is now = %s\n",location);


							//location = temp;

							printf("Returning = %s\n",temp);

							return temp;

							closedir(dir);
						};
					}

				
				};
			};

			closedir(dir);		// Stäng öppnat directory
			
		};

		//printf("%s\n",spaths[j]);
	}








	};
