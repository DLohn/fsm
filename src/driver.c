#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fsm.h"


#define MAX_INPUT_SIZE 256
enum {S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S_TRAP};

FSM my_fsm = {0};

/*
    2014 Minecraft username FSM: Accepts all strings that start with xX and end with Xx
*/
void fsm_def(FSM* fsm) {

    fsm->usetrapstate = 1;
    fsm->trapstate = S_TRAP;

    /* Detect start */
    add_transition_int(fsm, S0, 'x', S1);
    add_transition_int(fsm, S1, 'X', S2);

    /* Detect middle */
    add_transition_def(fsm, S2, S2);

    /* If X or Xx is typed, it's either (part of) the end or part of the middle */
    add_transition_int(fsm, S2, 'X', S3);

    add_transition_int(fsm, S3, 'X', S3);
    add_transition_int(fsm, S3, 'x', S10);
    add_transition_def(fsm, S3, S2);

    add_transition_int(fsm, S10, 'X', S3);
    add_transition_def(fsm, S10, S2);
    
}

int accepting_state(FSM* fsm) {
    return (fsm->currentstate == S9 || fsm->currentstate == S10);
}

int main() {
    
    char buf[MAX_INPUT_SIZE];
    int i;

    fsm_def(&my_fsm);

    while (1) {
        printf("Enter a string (blank to exit): ");
        if (fgets(buf, MAX_INPUT_SIZE, stdin) == NULL) {
            continue;
        }
        my_fsm.currentstate = S0;

        buf[strcspn(buf, "\n")] = 0;

        if (strlen(buf) == 0) {
            break;
        }

        for (i = 0; buf[i] != '\0'; i++) {
            /*printf("Character %d: %c\n", i + 1, buf[i]);*/
            fsm_int(&my_fsm, buf[i]);
        }

        if (accepting_state(&my_fsm)) {
            printf("String accepted!\n");
        }
        else {
            printf("String rejected!\n", my_fsm.currentstate);
        }

    }

    free_fsm(&my_fsm);

    printf("Blank string entered, exiting program.\n");
    return 0;

}