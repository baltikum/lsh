#include <stdio.h>
#include <string.h>
#include <stdlib.h> // getenv()

#include "parse.h"


int extractpath(Command *cmd) {

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


	foundpaths = savedpaths;

	return 0;
}