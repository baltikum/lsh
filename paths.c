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


	char savedpaths[numberofpaths][largestpathsize+1];

	char * spaths[15];
	char ** sspaths = spaths;

	char* pointer; // Pekare för att stega igenom copiedpath

	pointer = strtok(copiedpath,":"); // Stegar fram till :

	int i = 0;

	while( pointer != NULL ) {

		//char *savepath = pointer;
		spaths[i] = pointer;
		//printf("%s\n",pointer);
		//int j = 0;
		//while(*savepath != '\0') {
    	//	//printf("%c", *savepath);
		//	savedpaths[i][j] = *savepath;
    	//		savepath++;
		//	j++;
  		//} 
		//savedpaths[i][j] = '\0';
		//printf("SAVED PATH: %s\n",savedpaths[i]);
		++i;
		pointer = strtok(NULL,":"); // Stega framåt till :
	}



	
	DIR *dir;
	char* commandtosearchfor = *cmd->pgm->pgmlist;
	

	long commandlength = strlen(commandtosearchfor);

	//printf("Is searching:%s: and it should be %s   it is:%ld long\n",*cmd->pgm->pgmlist,commandtosearchfor,commandlength);


	for ( int i = 0; i < numberofpaths; i++ ) { // gå igenom alla våra paths tills att vi hittar

		//const char* retrievedpath = &savedpaths[i][0];
		const char* retrievedpath = spaths[i];

		//printf("\n\nRetreivedpath: %s\n\n",retrievedpath);

		dir = opendir(retrievedpath); //Öppna directory
		

		if ( dir == NULL ) {
			printf("Couldnt open path directory.\n"); // Krasch om det ej går annars kör vi
			//return -1;
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

	};





















}
