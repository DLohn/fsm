#include <stdio.h>
#include "fsm.h"
#include "jval.h"
#include "jrb.h"
#include "dllist.h"

int jvalcmp(Jval j1, Jval j2) {
    return memcmp(&j1, &j2, sizeof(Jval));
}

char defaultkey[8] = {0, 0, 1, 0, 0, 0, 0, 0};

enum {ADD, REMOVE, ST, ST2};
int add_remove_st_transition(FSM* fsm, uint startstate, Jval data_key, uint endstate, int func) {
    JRB state_transition_table;
    JRB inner_jrb;
    JRB found;
    Dllist dll;
    Dllist ptr;

    if (fsm->transition_table == 0) {
        fsm->transition_table = make_jrb();
    }

    state_transition_table = jrb_find_int(fsm->transition_table, startstate);
    if (state_transition_table == NULL) {
        if (func == ADD) {
            inner_jrb = make_jrb();
            jrb_insert_int(fsm->transition_table, startstate, new_jval_v((void*)inner_jrb));
        }
        else {
            return 0;
        }
    }
    else {
        inner_jrb = (JRB)(state_transition_table->val.v);
    }

    found = jrb_find_gen(inner_jrb, data_key, &jvalcmp);
    
    if (func == ST || func == ST2) {
        if (found == NULL) {
            memcpy(data_key.carray, defaultkey, sizeof(defaultkey));
            found = jrb_find_gen(inner_jrb, data_key, &jvalcmp);
        }

        if (found != NULL) {
            dll = (Dllist)(found->val.v);
            if (func == ST) {
                dll_traverse(ptr, dll) {
                    if(ptr->val.i == endstate) {
                        fsm->currentstate = endstate;
                        return 1;
                    }
                }
            }
            else if (func == ST2) {
                if (!dll_empty(dll)) {
                    fsm->currentstate = dll_last(dll)->val.i;
                    return 1;
                }
            }
        }

        if (fsm->usetrapstate) {
            fsm->currentstate = fsm->trapstate;
        }
        return 0;
    }
    else if (func == ADD) {
        if (found == NULL) {
            dll = new_dllist();
            dll_append(dll, new_jval_i(endstate));
            jrb_insert_gen(inner_jrb, data_key, new_jval_v(dll), &jvalcmp);
        }
        else {
            dll = (Dllist)(found->val.v);
            dll_traverse(ptr, dll) {
                if(ptr->val.i == endstate) {
                    return 0;
                }
            }
            dll_append(dll, new_jval_i(endstate));
        }
    }
    else if (func == REMOVE) {
        if (found != NULL) {
            dll = (Dllist)(found->val.v);
            dll_traverse(ptr, dll) {
                if(ptr->val.i == endstate) {
                    dll_delete_node(ptr);
                    return 0;
                }
            }
        }
    }
    return 0;
}

void free_fsm(FSM* fsm) {
    JRB ptr;
    JRB lst;
    JRB ptr2;
    if (fsm->transition_table == 0) {
        return;
    }

    jrb_traverse(ptr, fsm->transition_table) {
        lst = (JRB)(ptr->val.v);
        jrb_traverse(ptr2, lst) {
            free_dllist((Dllist)(ptr2->val.v));
        }
        jrb_free_tree(lst);
    }
    jrb_free_tree(fsm->transition_table);

}

Jval get_int_key(int data) {
    Jval key;
    memset(&key, 0, sizeof(key));
    memcpy(key.carray + sizeof(key.carray) - sizeof(data), &data, sizeof(data));
    return key;
}

Jval get_str_key(char* data) {
    Jval key;
    int i;
    memset(&key, 0, sizeof(key));
    for (i = 0; i < sizeof(key.carray) && data[i] != '\0'; i++) {
        key.carray[i] = data[i];
    }
    if(key.carray[0] == 0) {
        /* We are dealing with an empty string. 
           Use a sentinel value to distinguish "" and 0. */
        key.carray[1] = 1;
    }
    return key;
}

void add_transition_int(FSM* fsm, uint startstate, int data, uint endstate) {
    Jval key;
    key = get_int_key(data);
    add_remove_st_transition(fsm, startstate, key, endstate, ADD);
}

void add_transition_str(FSM* fsm, uint startstate, char* data, uint endstate) {
    Jval key;
    key = get_str_key(data);
    add_remove_st_transition(fsm, startstate, key, endstate, ADD);
}

void remove_transition_int(FSM* fsm, uint startstate, int data, uint endstate) {
    Jval key;
    key = get_int_key(data);
    add_remove_st_transition(fsm, startstate, key, endstate, REMOVE);
}

void remove_transition_str(FSM* fsm, uint startstate, char* data, uint endstate) {
    Jval key;
    key = get_str_key(data);
    add_remove_st_transition(fsm, startstate, key, endstate, REMOVE);
}

void add_transition_def(FSM* fsm, uint startstate, uint endstate) {
    Jval key;
    memcpy(key.carray, defaultkey, sizeof(defaultkey));
    add_remove_st_transition(fsm, startstate, key, endstate, ADD);
    return;
}
void remove_transition_def(FSM* fsm, uint startstate, uint endstate) {
    Jval key;
    memcpy(key.carray, defaultkey, sizeof(defaultkey));
    add_remove_st_transition(fsm, startstate, key, endstate, REMOVE);
}

int fsm_tostate_int(FSM* fsm, uint nextstate, int data) {
    Jval key;
    if (fsm->currentstate == nextstate) {
        return 1;
    }

    key = get_int_key(data);
    return add_remove_st_transition(fsm, fsm->currentstate, key, nextstate, ST);
}

int fsm_tostate_str(FSM* fsm, uint nextstate, char* data) {
    Jval key;
    if (fsm->currentstate == nextstate) {
        return 1;
    }

    key = get_str_key(data);
    return add_remove_st_transition(fsm, fsm->currentstate, key, nextstate, ST);
}

int fsm_int(FSM* fsm, int data) {
    Jval key;
    key = get_int_key(data);
    return add_remove_st_transition(fsm, fsm->currentstate, key, 0, ST2);
}

int fsm_str(FSM* fsm, char* data) {
    Jval key;
    key = get_str_key(data);
    return add_remove_st_transition(fsm, fsm->currentstate, key, 0, ST2);
}
