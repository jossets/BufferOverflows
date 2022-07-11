#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// gcc vuln.c -o vuln -m32 -fno-stack-protector -no-pie

int main (int argc, char **argv) {
    if (argc<2) {
        printf("Entreez une chaine à afficher\n");
        exit(1);
    }

    char msg[1024];
    strncpy (msg, argv[1], 1024);
    msg[1023]='\0';
    printf(msg);
    printf("\n");
    return(0);
}