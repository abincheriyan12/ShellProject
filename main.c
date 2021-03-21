// Abin M Cheriyan
// Assignment 2
// Shell Project

/*
a. cd <directory>
b. clr
c. dir <directory>
d. environ
e. echo <comment>
f. help
g. pause
h. quit
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

// built-in functions
#define CD   "cd"
#define CLR  "clr"
#define DR   "dir"
#define ENVIRON "environ"
#define ECHO    "echo"
#define HELP    "help"
#define PAUSE   "pause"
#define QUIT    "quit"
#define PATH    "path"

//global variables
char path[75][75];
char pwd[100];
char spath[100];   //shellpath
int pathsize = 1;

typedef struct{
    int  builtin;            //1 if the command is built-in
    char args[50][50];      //hold the arguments for a command
    int  argcount;
    int  out;                // >
    int  out1;               // >>
    int  in;                 // <
    int  pipe;               // |
    int  bgexecution;        //1 if the command should be executed in the background
}cmnd;

// builtin commands

void cd(cmnd);
void clr();
void dir(cmnd);
void environ(cmnd);
void echo(cmnd);
void help(cmnd);
void pausing(cmnd);
void error();
void exitshell(cmnd);
void PathChange (cmnd);
void handleevrythingelse(cmnd);
void pipehandler(char args[75][75], int argc, int number);

void initstruct(cmnd *in)
{
    in -> builtin = 0;
    in -> argcount = 0;
    in -> out = 0;
    in -> out1 = 0;
    in -> in = 0;
    in -> pipe = 0;
    in -> bgexecution = 0;
}

void builtinshandler(cmnd in)
{
    if (strcmp(in.args[0], CD) == 0)
    {
        //  at most one argument
        if (in.argcount > 1)
        {
            error();
        }
        else{
            cd(in);
        }
    }
    else if (strcmp(in.args[0], CLR) == 0)
    {
        if (in.argcount > 0)
        {
            error();
        }
        else{
            clr();
        }
        return;
    }
    else if (strcmp(in.args[0], DR) == 0)
    {
        dir(in);
    }
    else if (strcmp(in.args[0], ENVIRON) == 0)
    {
        environ(in);
    }
    else if (strcmp(in.args[0], ECHO) == 0)
    {
        echo(in);
    }
    else if (strcmp(in.args[0], HELP) == 0)
    {
        help(in);
    }
    else if (strcmp(in.args[0], PAUSE) == 0)
    {
        pausing(in);
    }
    else if (strcmp(in.args[0], QUIT) == 0)
    {
        exitshell(in);
    }
    else if (strcmp(in.args[0], PATH) == 0)
    {
        PathChange(in);
    }else
    {
        error();
    }
}

void parse(char *line, cmnd *in)
{
    const char *delims = " \n\t";
    char *buffer = strtok(line, delims);        //buffer to hold tokens
    if (buffer == NULL)
    {
        return;
    }
    if (strcmp(buffer, CD) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, CLR) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, DR) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, ENVIRON) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, ECHO) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, HELP) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, PAUSE) == 0)
    {
        in -> builtin = 1;
    }
    else if (strcmp(buffer, QUIT) == 0)
    {
        exit(0);
    }
    else if (strcmp(buffer, PATH) == 0)
    {
        in -> builtin = 1;
    }
    else{
        in -> builtin = 0;
    }

    strncpy(in -> args[0], buffer, 50);       //storing command in the argument array
    int i = 1;
    while((buffer = strtok(NULL, delims)) != NULL)  //loop to parse the rest of the input
    {
        switch (buffer[0])
        {
            case '>':
                if (buffer[0] == '>')
                {
                    if (buffer[1] == '>')
                    {
                        if(buffer[2] != '\0')
                        {
                            in -> builtin = -1;
                            return;
                        }
                        else
                            in -> out1 = i;
                    }
                    else if (buffer[1] != '\0')
                    {
                        in -> builtin = -1;
                        return;
                    }
                    else in -> out = i;
                }
                break;
            case '<':
                in -> in = i;
                if (buffer[1] != '\0')
                {
                    in -> builtin = -1;
                    return;
                }
                break;
            case '\t':
                break;

            case '\n':
                break;

            default:
                strncpy(in -> args[i++], buffer, 50);
                in -> argcount++;
        }
    }
    free(buffer);
}




int main(int argc, char *argv[])
{
    strcpy(path[0], "/bin");
    strcpy(path[1], "\0");
    getcwd(pwd, 100);
    getcwd(spath, 100);
    cmnd in;                    //command struct
    initstruct(&in);

    int batchmode;              //1 if batchmode, 0 for interactive batchmode
    FILE *file;
    if (argc == 2)
    {
        batchmode = 1;
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            printf("file cannot be opened\n");
            exit(1);
        }
    }
    else if (argc > 2)
    {
        printf(" >> arguments");
        exit(1);
    }
    else batchmode = 0;
    size_t size = 100;
    char *line = (char *)malloc(size * sizeof(char));//making memory for char array to hold usr inputs
    while(1)
    {
        printf("tuh00755 > ");
        if (batchmode)
        {
            if (getline(&line, &size, file) == -1)
            {
                break;
            }
        }
        else
        {
            fflush (stdin);
            fflush(stdin);
            getline(&line, &size, stdin);
        }
        if (strcmp(line, "\n") != 0)
        {
            parse(line, &in);

            if(in.builtin == -1)
            {
                error();
            }
            else if (in.builtin)  //once parsed,handle parsed command
            {
                builtinshandler(in);
            }
        }
        initstruct(&in);
    }
    fclose(file);
    free(line);
}
//Builtin Commands

//a. cd <directory>  BUILTIN COMMAND
void cd(cmnd in)
//if there are no arguments, the pwd is printed
// else chdir() is called. on success, the new pwd is printed
// else calls error func
{
    if (in.argcount == 0)
    {
        printf("%s \n ", pwd);
    }else
    {
        if (chdir(in.args[1]) == -1)
        {
            error();
        }
        else{
            getcwd(pwd, 100);
            printf("%s\n", pwd);
        }
    }
}

//b. clr - Prints 100 empty line
void clr()
{
    int i = 0;
    while( i <= 100)
    {
        printf("\n");
        i++;
    }
}

//  c. dir <directory> BUILTIN COMMAND
void dir(cmnd in)
{
    DIR *directory;
    struct dirent *read;
    if (in.argcount > 2)
    {
        error();
        return;
    }
    if (in.argcount == 0) //opens the the current directory if no argument given
    {
        directory = opendir(pwd);
    }
    else if ((in.argcount == 1) && ((in.out) || (in.out1)))
    {
        directory = opendir(pwd);
    }
    else{
        //opening the given directory
        directory = opendir(in.args[1]);
        if (directory == NULL)
        {
            error();
            return;
        }
    }
    read = readdir(directory);
    // Checking to see if redirrection is needed and printing
    if (in.out)         //   ">"
    {
        FILE *file = fopen(in.args[in.out], "w");

        while (read != NULL)
        {
            fprintf(file, "%s\n", read -> d_name);
            read = readdir(directory);
        }
        fclose(file);
    }
    else if (in.out1)   // ">>"
    {
        FILE *file = fopen(in.args[in.out1], "a");
        while (read != NULL)
        {
            fprintf(file, "%s\n", read -> d_name);
            read = readdir(directory);
        }
        fclose(file);
    }
    else
    {
        while (read != NULL)
        {
            printf("%s\n", read -> d_name);
            read = readdir(directory);
        }
    }
    closedir(directory);
}
//d. environ BUILTIN COMMAND
void environ(cmnd in)
        {
                if (in.argcount > 1)
                {
                    error();
                }
                else if (in.argcount == 1)
                {
                    FILE *fp;                                     //redirection
                    if (in.out)
                    {
                        fp = fopen(in.args [in.out], "w");        //truncate
                    }
                    else{
                        fp = fopen(in.args [in.out1], "a");   //append
                    }
                    if (fp == NULL)
                    {
                        error();
                        perror("\n");
                    }
                    fprintf(fp, " Path = %s ", path[0]);
                    int i = 1;
                    while(i < pathsize)
                    {
                        fprintf(fp, ":%s", path[i++]);
                    }
                    fprintf(fp, "\nshell = %s/ shell \n", spath);
                    fprintf(fp, "Pwd  =  %s\n", pwd);
                    fprintf(fp, "User = %s\n", getenv("USER"));
                    fprintf(fp, "Home = %s\n", getenv("HOME"));
                    fclose (fp);
                }
                else{                              //no redirection
                    printf("Path = %s", path[0]);
                    int i = 1;
                    while(i < pathsize)
                    {
                        printf(":%s", path[i++]);
                    }
                    printf("\nshell = %s/ shell \n", spath);
                    printf("Pwd  =  %s\n", pwd);
                    printf("User = %s\n", getenv("USER"));
                    printf("Home = %s\n", getenv("HOME"));
                }
        }

//e. echo <comment> BUILTIN COMMAND
        void echo(cmnd in)
{
    if (in.argcount == 0 || in.argcount > 2)
    {
        error();
        return;
    }
    if (in.out || in.out1)
    {
        FILE *file;
        if (in.out)
        {
            file = fopen(in.args[in.out], "w");
        }
        else
        {
            file = fopen(in.args[in.out1], "a");
        }

        if (file == NULL)
        {
            error();
            return;
        }
        fprintf(file, "%s\n", in.args[1]);
        fclose(file);
    }
    else
    {
        printf("%s\n", in.args[1]);
    }
}
// f. help BUILTIN COMMAND
void help(cmnd in)
{
    if (in.argcount > 1)
    {
        error();
        return;
    }
    char *line = NULL;
    size_t size = 100;
    FILE *hlp = fopen("readme", "r");
    if (hlp == NULL)
    {
        error();
        return;
    }
    getline(&line, &size, hlp);
    if (in.out || in.out1)
    {
        FILE *file;
        if (in.out)  // ">"
        {
            file = fopen(in.args[in.out], "w");
        }
        else{       // ">>"
            file = fopen(in.args[in.out1], "a");
        }

        if (file == NULL)
        {
            error();
            return;
        }
        fprintf(file, "%s", line);
        while(getline(&line, &size, hlp) != -1)
        {
            fprintf(file, "%s", line);
        }
        fclose(file);
    }
    else{
        printf("%s", line);
        while(getline(&line, &size, hlp) != -1)
        {
            printf("%s", line);
        }
    }
    fclose(hlp);
}

// g. pause BUILTIN COMMAND
void pausing(cmnd in)
{
    if (in.argcount)
    {
        error();
        return;
    }
    printf("Press enter to continue\n");
    char c;
    int i;

    do{                //exit and unpause the shell when only enter is pressed.
        fflush(stdin);
        i = 0;
        while ((c = getchar()) != '\n')
        {
            i++;
        }
    } while (i);
}

//Error function
// prints error
void error ()
{
    //printf("An error has occurred \n ");
    char *message = "An error has occured\n";
    write(STDERR_FILENO, message, strlen(message));
}

//Quit  BUILTIN COMMAND
void exitshell (cmnd in)
{
    if (in.argcount > 0)
    {
        error();
    }else
    {
        exit (0);
    }
}

//Changes the path
void PathChange(cmnd in)
{
    if (in.argcount == 0)
    {
        strcpy(path[0], "\0");
        return;
    }
    int i = 1;
    //this loop uses opendir() to check if the arguments given are existing dir
    while (i <= in.argcount)
    {
        DIR *directory = opendir(in.args[i]);
        if (directory == NULL)
        {
            error();
            return;
        }
        strcpy(path[i - 1], in.args[i]);
        i++;
        closedir(directory);
    }
    strcpy(path[i - 1], "\0");
}

void handleevrythingelse(cmnd in)
{
    if (strcmp(path[0], "\0") == 0)
    {
        error();
        return;
    }
    if (in.pipe > 0)
    {
        pipehandler(in.args, in.argcount, in.pipe);
        return;
    }
    /////
    ///fork
    ///etc

}
void pipehandler(char args[75][75], int argc, int number)
{
    if (!number)
    {
        error();
        return;
    }
    else
    {
        //////
    }

}
