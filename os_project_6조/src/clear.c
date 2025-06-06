#include <stdio.h>


void execute_clear() {

    printf("\033[2J\033[H");
    fflush(stdout); 
}
