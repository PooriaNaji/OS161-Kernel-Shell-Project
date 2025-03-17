#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <kern/types.h>
#include <string.h>


int main() {
   // pid_t pid = getpid();
	pid_t pid = getpid(); 
	printf("hiii");
	_exit(2);


    return pid;
}
