#ifndef FSM_H
#define FSM_H

#include <stdlib.h>
#include <string.h>
#include "jval.h"
#include "jrb.h"
#include "dllist.h"

typedef unsigned int uint;

extern char check_Jval[sizeof(Jval) == 8 ? 1 : -1];

typedef struct fsmdata {
    uint currentstate;
    char usetrapstate;
    uint trapstate;
    JRB transition_table;
} FSM;

void free_fsm(FSM* fsm);

/* Attempts a state transition. Returns 1 if successful, 0 if not. */
int fsm_tostate_int(FSM* fsm, uint nextstate, int data);
int fsm_tostate_str(FSM* fsm, uint nextstate, char* data);

/* Attempts a state transition. Chooses the most recently added transition rule if 
   multiple satisfactory transition rules exist.
   Returns 1 if successful, 0 if not. */
int fsm_int(FSM* fsm, int data);
int fsm_str(FSM* fsm, char* data);

void add_transition_int(FSM* fsm, uint startstate, int data, uint endstate);
void add_transition_str(FSM* fsm, uint startstate, char* data, uint endstate);

void remove_transition_int(FSM* fsm, uint startstate, int data, uint endstate);
void remove_transition_str(FSM* fsm, uint startstate, char* data, uint endstate);


/* Add/remove transitions that are triggered when the given data doesn't match */
void add_transition_def(FSM* fsm, uint startstate, uint endstate);
void remove_transition_def(FSM* fsm, uint startstate, uint endstate);

#endif
