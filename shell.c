#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char init[] = "";
char delimiters[] = " \t\r\n";
extern char **environ;
char *token;
pid_t pid;

void shell_handler(int signum)
{ 
	kill(pid,SIGKILL);
}

void shell_timer(int signum)
{ 
	kill(pid,SIGKILL);
}

int main() {
  // Stores the string typed into the command line.
  char command_line[MAX_COMMAND_LINE_LEN];
  char cmd_bak[MAX_COMMAND_LINE_LEN];
//     char path[];
//     char home[];

  // Stores the tokenized command line input.
  char *arguments[MAX_COMMAND_LINE_ARGS];
  
  char cwd[MAX_COMMAND_LINE_LEN];
  
  char path[MAX_COMMAND_LINE_LEN];

  while (true) {
    getcwd(cwd, sizeof(cwd));
    // Print the shell prompt.
      printf("%s%s", cwd, prompt);
    do{
      fflush(stdout);

      // Read input from stdin and store it in command_line. If there's an
      // error, exit immediately. (If you want to learn more about this line,
      // you can Google "man fgets")

      if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
          fprintf(stderr, "fgets error");
          exit(0);
      }
    }while(command_line[0] == 0x0A);  // while just ENTER pressed


    // If the user input was EOF (ctrl+d), exit the shell.
    if (feof(stdin)) {
      printf("\n");
      fflush(stdout);
      fflush(stderr);
      return 0;
    }

    // TODO:
    // 

    // 0. Modify the prompt to print the current working directory
    
    
    // 1. Tokenize the command line input (split it on whitespace)
    token = strtok(command_line, delimiters);
    int i = 0;
    while (token != NULL){
      arguments[i] = token;
      i++;
      token = strtok(NULL, delimiters);
    }
    
    // 2. Implement Built-In Commands
      
    if (!strcmp(arguments[0], "pwd")){
      if (cwd){
         printf("%s\n", cwd);
      }
    }
    else if (!strcmp(arguments[0], "cd")){
      
      if (arguments[1] == NULL){
         printf("Error:Missing one argument.");
       }
      else{
        chdir(arguments[1]);
      }
    }else if (!strcmp(arguments[0], "echo")){
      if (arguments[1] != NULL){
        char output[80];
        if (arguments[1][0] == '$') {
            memmove(arguments[1], arguments[1]+1, strlen(arguments[1]));
            strcpy(output, getenv(arguments[1]));
        }else{
          strcpy(output, arguments[1]);
        }
          int count = 2;
          for (count = 0; arguments[count] != NULL; count++){
            if (arguments[count][0] == '$') {
							memmove(arguments[count], arguments[count]+1, strlen(arguments[count]));
							strcpy(output, getenv(arguments[count]));
						}
            strcat(output, " ");
            strcat(output, arguments[count]);
          }
          printf("%s\n", output);
      }else{
        printf("Error: missing arguments");
      }
    }else if (!strcmp(arguments[0], "exit")){
      exit(0);
    }else if (!strcmp(arguments[0], "env")){
      char** temp_env = environ;
      while (*temp_env != NULL){
        printf("%s", *temp_env);
        temp_env++;
      }
    }else if (!strcmp(arguments[0], "setenv")){
      char* tmp = strtok(arguments[1], "=");
      char* variable = tmp;
      char* value;
      while (tmp != NULL){
        value = tmp;
        tmp = strtok(NULL, "=");     
      }
     
      setenv(variable, value,1);
      // 3. Create a child process which will execute the command line input
    }
    else{
      pid = fork();
      
      int isParallel;
      
      if (arguments[1]){
        if (!strcmp(arguments[1], "&")){
          isParallel = 1;
          arguments[1] = NULL;
        }else{
          isParallel = 0;
        }
      }
      if (pid <0){
        printf("Error in fork");
        exit(1);
      }else if (pid == 0){
        signal(SIGINT, shell_handler);		
        int fd0;
        int out_val=0;
        char out_str[64];
        int count;
        
        for(count=0;arguments[i] != '\0';count++){
              if(!strcmp(arguments[count],">")){        
                  arguments[count]=NULL;
                  strcpy(out_str,arguments[count+1]);
                  out_val=2;           
              }                       
          }
        
        if (out_val){
          int fd1 ;
          if ((fd1 = creat(out_str , 0644)) < 0) {
              perror("Error:Cant open file");
              exit(0);
          }           

          dup2(fd1, 1);
          close(fd1);
        }
        
        execvp(arguments[0], &arguments[0]);
        perror("Can't Execute\n");  
				exit(1); 
      }
     

    // 4. The parent process should wait for the child to complete unless its a background process
      else{
        signal(SIGINT, shell_handler);
        signal(SIGALRM,shell_timer);
        
        alarm(10);
        
        if (isParallel == 1){
          isParallel=0;
        }
        else {
          wait(NULL);
        }
      }

    // Hints (put these into Google):
    // man fork
    // man execvp
    // man wait
    // man strtok
    // man environ
    // man signals

    // Extra Credit
    // man dup2
    // man open
    // man pipes
  }
// This should never be reached.
  return -1;
}
}


