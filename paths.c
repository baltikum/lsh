#include <stdio.h>
#include <string.h>
#include <stdlib.h> // getenv()

#include "parse.h"



#include <dirent.h>


const char* extractpath(Command *cmd, int* isavailable) {

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
	char* pointer; // Pekare för att stega igenom copiedpath

	pointer = strtok(copiedpath,":"); // Stegar fram till :

	int i = 0;

	while( pointer != NULL ) {

		char *savepath = pointer;
		int j = 0;
		while(*savepath != '\0') {
    		//printf("%c", *savepath);
			savedpaths[i][j] = *savepath;
    		savepath++;
			j++;
  		} 
		savedpaths[i][j] = '\0';
		printf("SAVED PATH: %s\n",savedpaths[i]);
		++i;
		pointer = strtok(NULL,":"); // Stega framåt till :
	}


	
	DIR *dir;
	char* commandtosearchfor = "ls";//*cmd->pgm->pgmlist; // fiktivt kommando in
	

	long commandlength = strlen(commandtosearchfor);

	printf("Is searching:%s:  it is:%ld long\n",*cmd->pgm->pgmlist,commandlength);

	for ( int i = 0; i < numberofpaths; i++ ) { // gå igenom alla våra paths tills att vi hittar

		const char* retrievedpath = &savedpaths[i][0];

		printf("%s\n",retrievedpath);

		dir = opendir(retrievedpath); //Öppna directory

		if ( dir == NULL ) {
			printf("Couldnt open path directory"); // Krasch om det ej går annars kör vi
			//return -1;
		} else {

			struct dirent *dirpointer; // directory entry struct

			while( dirpointer = readdir(dir)) { // Loop läser in entrys ifrån directory

				char* pointer = dirpointer->d_name;

				//printf("%s",pointer);

				long pointerlength = strlen(pointer);

				if ( commandlength == pointerlength) { // Fortsätt om de är lika långa
					long k = 0;
					while ( *pointer == *commandtosearchfor ) { // Kolla char för char

						k++;
						pointer++;
						commandtosearchfor++;

					};

					if ( k == (commandlength+1) ) { // Kollar om det är matchning
						
						*isavailable = 1;
						printf("Command found in %s\n",retrievedpath);
						return retrievedpath; // return path till directory där vi hittade binary
					};
				};
			};

			closedir(dir);		// Stäng öppnat directory
		
			
		};

	};





















}