
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

char* PWD; //PWD holds the user's working directory
char* shortName; //just the curDir name, not whole path
char *PATH[6]; //PATH contains a sequence of directories indicating where to search for commands issued by the user.
int sizeOfPath = 6;
//the longest pathname on both ext3 (default FS on Ubuntu) and HFS Plus (defuat on OS X) is 255 characters.

char buffer[512];//holds the text the user enters before it's processed.
int const MAX_ARGS = 100;
int const MAX_ARGSONE = 101;

/*
 __  __        ____               _ 
 |  \/  |_   _ / ___|_ __ ___   __| |
 | |\/| | | | | |   | '_ ` _ \ / _` |
 | |  | | |_| | |___| | | | | | (_| |
 |_|  |_|\__  |\____|_| |_| |_|\__ _|   
 |___/   */                    

typedef struct { 
    char* name; 
    int argc; 
    char* argv[101]; 
} myCommand; 


myCommand* elCMD;
/*
 ____ ____ ____ ____ ____ ____ ____ ____ ____ 
 ||F |||u |||n |||c |||t |||i |||o |||n |||s ||
 ||__|||__|||__|||__|||__|||__|||__|||__|||__||
 |/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\| */

// myCommand Functions
myCommand* getCommand(char stringystring[]);
void testCommand();

//Internal Varible Management Fuctions
void changeDir();
void echo();
void initialize();
void finish();
void ls();
void changethedir();

// Executing Command Functions
char* findCommand(char* name);
void forkAndRun();
void executeCommand();

// Change Directory Functions
void changeShortName();
void changePWD();
void addPWDtoPath();

//Signal Handler
void sighandler(int sig);

/************************************************************************
 *                                                                      *
 *      Function: Main                                                  *
 *                                                                      *
 *      Purpose: It runs the entire program. It's main                  *
 *                                                                      *
 ************************************************************************/

int main (int argc, const char * argv[]) {
	
    signal(SIGINT, sighandler); 
    initialize();    
	
    while(1)
	{
        // 1. Print Shell Prompt
        printf("\n%s$ ",shortName);
        
        //2. Get input from user
		fgets(buffer, 512, stdin);
        
        //3. Parse Command
		elCMD = getCommand(buffer);
        
        //4. Run Command
        executeCommand(elCMD);
        
        //5. Deallocate Memory
        free(elCMD);

	}//end main while loop
    
    finish();
    return 0;
}

/************************************************************************
 *                                                                      *
 *      Function: executeCommand()                                      *
 *                                                                      *
 *      Purpose: Process the command and do it                          *
 *                                                                      *
 ************************************************************************/


void executeCommand()
{
 //   printf("\n----executeCommand()----\n");
    if (elCMD->name != NULL)
    {
    if (strncmp(elCMD->name,"cd",2) == 0) // Option 1. Change Directory Command
    {    
        changeDir(elCMD);
    }
    else if (strncmp(elCMD->name,"echo",4) == 0) // Option 2. Echo Command
    {
        if (strncmp(elCMD->argv[1],"$PWD",4) == 0) //Op 2a. Echo cur directory
            printf("\nThe current working directory is: %s \n", PWD);
        else
            echo(elCMD); //Op 2b. General Echo
    }
    else if (strncmp(elCMD->name,"ls",2) == 0) //Op. 3 ls
    {
        ls(elCMD);
    }
    else
    {
      forkAndRun(elCMD); // Option 4. Find and Execute Command
    }
        
    } //end != NULL
    else printf("  "); 
}

/************************************************************************
 *                                                                      *
 *      Function: initialize()                                          *
 *                                                                      *
 *      Purpose: Intialize the global varibles                          *
 *                                                                      *
 ************************************************************************/

void initialize()
{
    //1. Allocate Memory
    PWD = malloc(256 * sizeof(char));
    shortName = malloc(256 * sizeof(char));
    PATH[5] = malloc(256 * sizeof(char));
    
    //2. Assign Starting Values for working directory
    PWD = "/Users/Jared/"; //starting location
	shortName = "Jared";
    
    //3. Assign starting paths
    PATH[0] = "/bin/";
    PATH[1] = "/usr/bin/";
    PATH[2] = "/usr/local/bin/";
    PATH[3] = "/Developer/usr/bin/";
    PATH[4] = "/home/harrisj/bin/";
    PATH[5] = "/Users/Jared/";
}
/************************************************************************
 *                                                                      *
 *      Function: findCommand()                                         *
 *                                                                      *
 *      Purpose: find the file to execute                               *
 *                                                                      *
 ************************************************************************/

char* findCommand(char* name)
{
    // 1. Setup Varibles
    char* location = malloc(255 * sizeof(char)); //temp string for the path
    FILE* fp; //pointer to file
    
    // 2. Search paths for executable file
    for(int i = 0; i < sizeOfPath; i++)
    {   
        strcpy(location,PATH[i]); //get path
        strncat(location,name,256); //append name to path. It's 256 because that's the longest possible filename 
        
        // 1. Try to find file
        fp = fopen(location, "r");
        
        //2. See if it's valid
        
        //2a. If valid end loop
        if (fp != NULL) 
        {
            i = sizeOfPath; 
        }
        //3b. If not, for loop will repeat till done
    }
    
    //If the file can not be found, return NULL
    if (fp == NULL)
        location = NULL;
    
    fclose(fp); //make sure to close file
    return location; //if file was found, pointer is not NULL
   
}

/************************************************************************
 *                                                                      *
 *      Function: forkAndRun()                                          *
 *                                                                      *
 *      Purpose: find the file to execute and fork it                   *
 *                                                                      *
 ************************************************************************/


void forkAndRun()
{
    //Find command
    char* location = findCommand(elCMD->name);
    
    
    //If invalid, display error
    if (location == NULL)
        strerror(2);
    else //else fork it!
    {
            //1. Fork
            int pid = fork();
        
            //2. If pid !=0 then it's the parent
            if(pid != 0)
            {
                wait(NULL);
            }
            //3.  Else we're the child and can excuete the program
            else 
            {
                int rv = execv(location, elCMD->argv);
            }
        
    }
}
/************************************************************************
 *                                                                      *
 *      Function: ls()                                          *
 *                                                                      *
 *      Purpose: perform ls                                             *
 *                                                                      *
 ************************************************************************/
    
void ls()
{
    //Fix Command for ls. We need to add the working directory to the ls command
    elCMD->argv[elCMD->argc] = malloc(256 * sizeof(char)); //1. Allocate Memory for String
    
    strcpy(elCMD->argv[elCMD->argc],PWD); //2. Copy PWD to the argvuments array
    
    (elCMD->argc)++; //3. increment argc
    
// testCommand(elCMD);
    
    forkAndRun(); //4. run it
        
}

        
/************************************************************************
 *                                                                      *
 *      Function: getCommand()                                          *
 *                                                                      *
 *      Purpose: parse a string into a command                          *
 *                                                                      *
 ************************************************************************/
	
myCommand* getCommand(char stringystring[]) 
{
    // 1. Allocate Memory
    myCommand* elCmd = malloc(sizeof(myCommand));
		
    // 2. Tokenize
    
    if (strcmp(stringystring, "\n") == 0 || strcmp(stringystring, "\0") == 0)
    {
        elCmd->name = NULL;
    }
    else
    {    
    // 2a. Get name of function
    elCmd->name = strtok(stringystring, " ");
    elCmd->argv[0] = elCmd->name; //put name in first argv
    elCmd->argc = 1;
		
    // 2b. Get Arguments
    char* buffer;
    do 
    {   //Tokenize arguments
        buffer = strtok(NULL, " ");
        if (buffer != NULL) 
        {
            //Loop thru arguments
            elCmd->argv[elCmd->argc] = buffer; //assign to argv
            (elCmd->argc)++; //increment counter
        }

    } while (buffer != NULL);
        
    //3. Trim \n off end of last argument
    elCmd->argv[elCmd->argc - 1][strlen(elCmd->argv[elCmd->argc - 1]) - 1] = '\0'; //trim \n off last argument
        
    }
    
    return elCmd;
	}
/************************************************************************
 *                                                                      *
 *      Function: changeDir()                                           *
 *                                                                      *
 *      Purpose: Change the working directory                           *
 *                                                                      *
 ************************************************************************/

void changeDir() 
{	    
    // Attempt to open directory
    DIR *dir;
    dir = opendir(elCMD->argv[1]); 
    
    // Case 1. Absolute Directory Name e.g. /System/Library/CoreServices/
    if (dir != NULL)
    {
        closedir(dir);
        changethedir();   
    }
    
    // Case 2. Relative Directory e.g. CoreServices
    else {
        // A. Get Full Name
        char* fullName = malloc(255 * sizeof(char)); //temp string for the path
        strcpy(fullName,PWD); //get current directory
        strncat(fullName,elCMD->argv[1],256); //add new directory name to current directory to get full name
        
        // B. See if full name exists
        dir = opendir(fullName);
        if (dir != NULL)
        {
            // If so, modify the command so PWD can be changed.
            closedir(dir);
            elCMD->argv[1] = NULL;
            elCMD->argv[1] = malloc(255 * sizeof(char));
            strcpy(elCMD->argv[1], fullName); //copy full name into argv[1] for changethedir()
            changethedir();
        }
        else 
        {
         printf("-jsh:--%s--folder not found",elCMD->argv[1]);   
        }
        
    } //end second else	
}

void changethedir()
{
    // A. Copy the new directory into PWD
    changePWD(elCMD);
    
    // B. Parse the directory and extract the short name
    changeShortName();
    
    // C. Add the new directory to the Paths
    addPWDtoPath();
}

/************************************************************************
 *                                                                      *
 *      Function: addPWDtoPath()                                        *
 *                                                                      *
 *      Purpose: add PWD to the paths                                   *
 *                                                                      *
 ************************************************************************/
void addPWDtoPath()
{
    PATH[5] = NULL;
    PATH[5] = malloc(256 * sizeof(char));
    strcpy(PATH[5],PWD);
}

/************************************************************************
 *                                                                      *
 *      Function: changePWD()                                           *
 *                                                                      *
 *      Purpose: change PWD                                             *
 *                                                                      *
 ************************************************************************/

void changePWD(myCommand* elCmd)
{
    PWD = NULL;
    PWD = malloc(256 * sizeof(char));
    strcpy(PWD, elCmd->argv[1]);
    
    int c = strlen(PWD);
    //Check to see if PWD ends with / but don't go off edge
    if (PWD[c - 1] != '/' && c < 256)
    {
        PWD[c] = '/';
        PWD[c + 1] = '\ec0';
    }
 //   printf("strlen() - 1: %c",PWD[strlen(PWD) - 1]);
}

/************************************************************************
 *                                                                      *
 *      Function: changeShortName()                                     *
 *                                                                      *
 *      Purpose: change the short name to be in line with what PWD is   *
 *                                                                      *
 ************************************************************************/

void changeShortName()
{
    shortName = NULL;
    shortName = malloc(256 * sizeof(char));
    
    int i = strlen(PWD) - 1; //start at end of string. For some reason strlen seems to be off by two
    // A. Cycle thru characters until a '/' is found. Ignore a terminating '/'. It's a do while loop because of situations like "/Users/Jared/Documents/" which end with a '/'
    do 
    {
        i--;
    } while (PWD[i] != '/' && i > -1);
    
    i++; // do while loop will end on the '/' we want to start the character after the '/'
    
    // B. Copy shortname to shortName, ignore terminating /
    for(int k = 0; i < strlen(PWD) && PWD[i] != '/'; i++)
    {
        shortName[k] = PWD[i];
        k++;
    }
    
}

/************************************************************************
 *                                                                      *
 *      Function: echo()                                                *
 *                                                                      *
 *      Purpose: print out what was typed                               *
 *                                                                      *
 ************************************************************************/
	
void echo()
{
    for (int i = 1; i < elCMD->argc; i++) 
    {
        printf("%s ",elCMD->argv[i]);
    }
}

/************************************************************************
 *                                                                      *
 *      Function: finish()                                              *
 *                                                                      *
 *      Purpose: clean up the program at the end                        *
 *                                                                      *
 ************************************************************************/

void finish()
{
    free(PWD);
    free(shortName);
    free(PATH[5]);
}

/************************************************************************
 *                                                                      *
 *      Function: sighandler()                                          *
 *                                                                      *
 *      Purpose: handle signals                                         *
 *                                                                      *
 ************************************************************************/


void sighandler(int sig)
{
    // Ignoring SIGINT
    
    signal(sig, SIG_IGN);                   // disable signal
    free(elCMD); //clear the current command and start over
    printf("\n");                           // Print dumby stuff to to user
    elCMD = getCommand("\n");       //Turn the command into a dummy command that doesn't do anything so loop restarts
    signal(SIGINT, sighandler);   // reinstall the signal handler

}

/************************************************************************
 *                                                                      *
 *      Function: testCommand()                                         *
 *                                                                      *
 *      Purpose: test the command                                       *
 *                                                                      *
 ************************************************************************/


void testCommand()
{
    printf("\n\n---testCommand()---\n\n");
    if (elCMD->name == NULL)
        printf("There was no command entered!\n");
    else 
    {
    printf("Name: %s\n",elCMD->name);
    printf("There were %d arguments. They are the following:\n",elCMD->argc - 1);
    for (int i = 0; i < elCMD->argc; i++) 
        {
        printf("argv[%d]:---%s---\n",i,elCMD->argv[i]);
        }
    printf("\nelCMD->argc: %d\n",elCMD->argc);
    }
}

	

