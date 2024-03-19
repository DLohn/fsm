#ifndef FSM_H
#define FSM_H

#include <stdlib.h>
#include <string.h>
#include "jval.h"
#include "jrb.h"
#include "dllist.h"

typedef unsigned int uint;

extern char check_Jval[sizeof(Jval) == 8 ? 1 : -1];

union data4b { /* Must be 4 bytes */
    int ival;
    float fval;
    char sval[4];
};
typedef union data4b data4b;
extern char check_tr_data[sizeof(data4b) == 4 ? 1 : -1];

struct TR8B { /* Must be 8 bytes */
    uint deststate;
    data4b trdat;
};
typedef struct TR8B TR8B;
extern char check_FSMTR[sizeof(TR8B) == 8 ? 1 : -1];

typedef struct fsmdata {
    uint currentstate;
    JRB transition_table;
} FSM;

void free_fsm(FSM* fsm);

/* Attempts a state transition. Returns 1 if successful, 0 if not. */
int fsm_tostate_int(FSM* fsm, uint nextstate, int data);
int fsm_tostate_str(FSM* fsm, uint nextstate, char* data);

/* Attempts a state transition. Chooses the most recent transition rule if 
   multiple satisfactory transition rules exist.
   Returns 1 if successful, 0 if not. */
int fsm_int(FSM* fsm, int data);
int fsm_str(FSM* fsm, char* data);


void add_transition_int(FSM* fsm, uint startstate, int data, uint endstate);
void set_transition_str(FSM* fsm, uint startstate, char* data, uint endstate);

void remove_transition_int(FSM* fsm, uint startstate, int data, uint endstate);
void remove_transition_str(FSM* fsm, uint startstate, char* data, uint endstate);



#endif
