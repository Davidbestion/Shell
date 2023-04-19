#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fenv.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>

int main(){
    char cwd[] = "/";
    while(1){
       
    }
    return 0;
}
char* ProcessInput(char input[]){

    char input[] = "This is a test string";
    char *token;
    char *array[10];
    int i = 0;

    token = strtok(input, " ");
    while (token != NULL) {
    array[i++] = token;
    token = strtok(NULL, " ");
    }
    return array;
}