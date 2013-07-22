/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION.
 *************************************************************************/

/* =======================================================================
                                reorder.h
   Need to add comments here
======================================================================= */

#ifndef REORDER_H
#define REORDER_H

typedef uint64_t tc_number;
typedef struct cons_struct cons;
typedef struct agent_struct agent;
typedef struct action_struct action;
typedef struct condition_struct condition;
typedef cons list;
typedef struct symbol_struct Symbol;

extern bool reorder_action_list (agent* thisAgent, action **action_list, tc_number lhs_tc);
extern bool reorder_lhs (agent* thisAgent, condition **lhs_top,
						 condition **lhs_bottom, bool reorder_nccs);
extern void init_reorderer (agent* thisAgent);

/* this prototype moved here from osupport.cpp -ajc (5/3/02) */
extern list *collect_root_variables(agent* thisAgent, condition *, tc_number, bool);

#endif


