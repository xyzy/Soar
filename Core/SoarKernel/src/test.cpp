/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION.
 *************************************************************************/

/*************************************************************************
 *
 *  file:  rhs.cpp
 *
 * =======================================================================
 *                    Test Utilities
 * This file contains various utility routines for tests.
 *
 * =======================================================================
 */

#include "test.h"
#include "kernel.h"
#include "symtab.h"

/* =================================================================

                      Utility Routines for Tests

================================================================= */

/* --- This just copies a consed list of tests and returns
 *     a new copy of it. --- */

list *copy_test_list (agent* thisAgent, cons *c) {
  cons *new_c;

  if (!c) return NIL;
  allocate_cons (thisAgent, &new_c);
  new_c->first = copy_test (thisAgent, static_cast<test>(c->first));
  new_c->rest = copy_test_list (thisAgent, c->rest);
  return new_c;
}

inline bool is_test_type_with_no_referent(TestType test_type)
{
  return ((test_type == GOAL_ID_TEST) ||
          (test_type == CONJUNCTIVE_TEST) ||
          (test_type == IMPASSE_ID_TEST) ||
          (test_type == DISJUNCTION_TEST) ||
          (test_type == BLANK_TEST));
}

inline test make_test_without_refcount(agent* thisAgent, Symbol * sym, TestType test_type)
{
  test new_ct;

  if (!sym && (!is_test_type_with_no_referent(test_type)))
    return make_blank_test();

  allocate_with_pool (thisAgent, &thisAgent->test_pool, &new_ct);
  new_ct->type = test_type;
  new_ct->data.referent = sym;
  new_ct->original_test = NULL;

  return new_ct;
}

inline test make_test(agent* thisAgent, Symbol * sym, TestType test_type) // is this equivalent to the macro above??
{
  test new_ct;

  if (!sym && (!is_test_type_with_no_referent(test_type)))
    return make_blank_test();

  new_ct = make_test_without_refcount(thisAgent, sym, test_type);
  if (sym)
  {
    symbol_add_ref(thisAgent, sym);
  }

  return new_ct;
}

/* ----------------------------------------------------------------
   Takes a test and returns a new copy of it.
---------------------------------------------------------------- */

test copy_test (agent* thisAgent, test t) {
  Symbol *referent;
  test new_ct;

  if (test_is_blank(t))
    return make_blank_test();

  switch(t->type) {
    case GOAL_ID_TEST:
    case IMPASSE_ID_TEST:
      new_ct = make_test(thisAgent, NIL, t->type);
      break;
    case DISJUNCTION_TEST:
      new_ct = make_test(thisAgent, NIL, t->type);
      new_ct->data.disjunction_list =
          copy_symbol_list_adding_references (thisAgent, t->data.disjunction_list);
      break;
    case CONJUNCTIVE_TEST:
      new_ct = make_test(thisAgent, NIL, t->type);
      new_ct->data.conjunct_list = copy_test_list (thisAgent, t->data.conjunct_list);
      break;
    default:  /* relational tests other than equality */
      new_ct = make_test(thisAgent, t->data.referent, t->type);
      break;
  }
  if (t->original_test) {
    new_ct->original_test = copy_test(thisAgent, t->original_test);
  }
  return new_ct;
}

/* ----------------------------------------------------------------
   Same as copy_test(), only it doesn't include goal or impasse tests
   in the new copy.  The caller should initialize the two flags to FALSE
   before calling this routine; it sets them to TRUE if it finds a goal
   or impasse test.
---------------------------------------------------------------- */

test copy_test_removing_goal_impasse_tests (agent* thisAgent, test t,
    Bool *removed_goal,
    Bool *removed_impasse) {
  cons *c;
  test new_t, temp;

  switch(t->type) {
    case EQUALITY_TEST:
      return copy_test (thisAgent, t);
      break;
    case GOAL_ID_TEST:
      *removed_goal = TRUE;
      return make_blank_test();
    case IMPASSE_ID_TEST:
      *removed_impasse = TRUE;
      return make_blank_test();

    case CONJUNCTIVE_TEST:
      new_t = make_blank_test();
      for (c=t->data.conjunct_list; c!=NIL; c=c->rest) {
        temp = copy_test_removing_goal_impasse_tests (thisAgent, static_cast<test>(c->first),
            removed_goal,
            removed_impasse);
        if (! test_is_blank(temp))
          add_new_test_to_test (thisAgent, &new_t, temp);
      }
      if (new_t->type==CONJUNCTIVE_TEST)
      {
        new_t->data.conjunct_list =
            destructively_reverse_list (new_t->data.conjunct_list);
      }
      if (t->original_test) {
        new_t->original_test = copy_test(thisAgent, t->original_test);
      }
      return new_t;

    default:  /* relational tests other than equality */
      return copy_test (thisAgent, t);
  }
}

/* ----------------------------------------------------------------
   Deallocates a test.
---------------------------------------------------------------- */

void deallocate_test (agent* thisAgent, test t) {
  cons *c, *next_c;

  if (test_is_blank(t)) return;

  switch (t->type) {
  case GOAL_ID_TEST:
  case IMPASSE_ID_TEST:
    break;
  case DISJUNCTION_TEST:
    deallocate_symbol_list_removing_references (thisAgent, t->data.disjunction_list);
    break;
  case CONJUNCTIVE_TEST:
    c = t->data.conjunct_list;
    while (c) {
      next_c = c->rest;
      deallocate_test (thisAgent, static_cast<test>(c->first));
      free_cons (thisAgent, c);
      c = next_c;
    }
    break;
  default: /* relational tests other than equality */
    symbol_remove_ref (thisAgent, t->data.referent);
    break;
  }
  if (t->original_test)
    deallocate_test (thisAgent, t->original_test);
  free_with_pool (&thisAgent->test_pool, t);
}

/* ----------------------------------------------------------------
   Destructively modifies the first test (t) by adding the second
   one (add_me) to it (usually as a new conjunct).  The first test
   need not be a conjunctive test.
---------------------------------------------------------------- */

void add_new_test_to_test (agent* thisAgent,
               test *t, test add_me, test add_me_original) {
  test ct = 0, ct_orig = 0;
  cons *c, *c_orig;

  if (test_is_blank(add_me)) return;

  // Check if original variable name is unique.  If not, change it before adding.
  // Must be able to get name of instantiation (not production bc the productions
  // could come from two different firings)

  if (test_is_blank(*t)) {
    *t = add_me;
    return;
  }
  ct = *t;
  if (ct->type!=CONJUNCTIVE_TEST) {
    ct = make_test(thisAgent, NIL, CONJUNCTIVE_TEST);
    allocate_cons (thisAgent, &c);
    ct->data.conjunct_list = c;
    c->first = *t;
    c->rest = NIL;

    if (add_me_original)
    {
      ct_orig = make_test(thisAgent, NIL, CONJUNCTIVE_TEST);
      allocate_cons (thisAgent, &c_orig);
      ct_orig->data.conjunct_list = c_orig;
      c_orig->first = (*t)->original_test;
      c_orig->rest = NIL;
      ct->original_test = ct_orig;
    }
    *t = ct;
  }
  // Debug | remove
  if (add_me->type==CONJUNCTIVE_TEST) {
    print(thisAgent, "CONJUNCTIVE TEST SHOULD NOT BE HERE!!!!!!!");
  }
  /* --- now add add_me to the conjunct list --- */
  allocate_cons (thisAgent, &c);
  c->first = add_me;
  c->rest = ct->data.conjunct_list;
  ct->data.conjunct_list = c;

  if (add_me_original)
  {
    ct_orig = ct->original_test;
    allocate_cons (thisAgent, &c_orig);
    c_orig->first = add_me_original;
    c_orig->rest = ct_orig->data.conjunct_list;
    ct_orig->data.conjunct_list = c_orig;

    add_me->original_test = add_me_original;
  }
}


/* ----------------------------------------------------------------
   Same as add_new_test_to_test(), only has no effect if the second
   test is already included in the first one.
---------------------------------------------------------------- */

void add_new_test_to_test_if_not_already_there (agent* thisAgent, test *t, test add_me, bool neg) {
  test ct;
  cons *c;

  if (tests_are_equal (*t, add_me, neg)) {
    deallocate_test (thisAgent, add_me);
    return;
  }

    ct = *t;
    if (ct->type == CONJUNCTIVE_TEST)
      for (c=ct->data.conjunct_list; c!=NIL; c=c->rest)
        if (tests_are_equal (static_cast<test>(c->first), add_me, neg)) {
          deallocate_test (thisAgent, add_me);
          return;
        }

  add_new_test_to_test (thisAgent, t, add_me);
}

/* ----------------------------------------------------------------
   Returns TRUE iff the two tests are identical.
   If neg is true, ignores order of members in conjunctive tests
   and assumes variables are all equal.
---------------------------------------------------------------- */

Bool tests_are_equal (test t1, test t2, bool neg) {
  cons *c1, *c2;

    if (t1->type==EQUALITY_TEST)
    {
      if (t2->type!=EQUALITY_TEST)
        return FALSE;

      if (t1->data.referent == t2->data.referent) /* Warning: this relies on the representation of tests */
        return TRUE;

      if (!neg)
        return FALSE;

      // ignore variables in negation tests
      Symbol* s1 = t1->data.referent;
      Symbol* s2 = t2->data.referent;

      if ((s1->var.common_symbol_info.symbol_type == VARIABLE_SYMBOL_TYPE) && (s2->var.common_symbol_info.symbol_type == VARIABLE_SYMBOL_TYPE))
      {
        return TRUE;
      }
      return FALSE;
    }

    if (t1->type != t2->type)
      return FALSE;

    switch(t1->type) {
      case GOAL_ID_TEST:
        return TRUE;

      case IMPASSE_ID_TEST:
        return TRUE;

      case DISJUNCTION_TEST:
        for (c1 = t1->data.disjunction_list, c2 = t2->data.disjunction_list; (c1!=NIL) && (c2!=NIL); c1 = c1->rest, c2 = c2->rest)
        {
          if (c1->first != c2->first)
            return FALSE;
        }
        if (c1 == c2)
          return TRUE;  /* make sure they both hit end-of-list */
        return FALSE;

      case CONJUNCTIVE_TEST:
        // bug 510 fix: ignore order of test members in conjunctions
      {
        std::list<test> copy2;
        for (c2 = t2->data.conjunct_list; c2 != NIL; c2 = c2->rest)
          copy2.push_back(static_cast<test>(c2->first));

        std::list<test>::iterator iter;
        for (c1 = t1->data.conjunct_list; c1 != NIL; c1 = c1->rest)
        {
          // check against copy
          for(iter = copy2.begin(); iter != copy2.end(); ++iter)
          {
            if (tests_are_equal(static_cast<test>(c1->first), *iter, neg))
              break;
          }

          // iter will be end if no match
          if (iter == copy2.end())
            return FALSE;

          // there was a match, remove it from unmatched
          copy2.erase(iter);
        }

        // make sure no unmatched remain
        if (copy2.empty())
          return TRUE;
      }
      return FALSE;

      default:  /* relational tests other than equality */
        if (t1->data.referent == t2->data.referent)
          return TRUE;
        return FALSE;
    }
  }

/* ----------------------------------------------------------------
   Returns a hash value for the given test.
---------------------------------------------------------------- */

uint32_t hash_test (agent* thisAgent, test t) {
  cons *c;
  uint32_t result;

  if (test_is_blank(t))
    return 0;

  switch (t->type) {
    case EQUALITY_TEST: return t->data.referent->common.hash_id;
    case GOAL_ID_TEST: return 34894895;  /* just use some unusual number */
    case IMPASSE_ID_TEST: return 2089521;
    case DISJUNCTION_TEST:
      result = 7245;
      for (c=t->data.conjunct_list; c!=NIL; c=c->rest)
        result = result + static_cast<Symbol *>(c->first)->common.hash_id;
      return result;
    case CONJUNCTIVE_TEST:
      result = 100276;
      // bug 510: conjunctive tests' order needs to be ignored
      //for (c=ct->data.disjunction_list; c!=NIL; c=c->rest)
      //  result = result + hash_test (thisAgent, static_cast<constraint>(c->first));
      return result;
    case NOT_EQUAL_TEST:
    case LESS_TEST:
    case GREATER_TEST:
    case LESS_OR_EQUAL_TEST:
    case GREATER_OR_EQUAL_TEST:
    case SAME_TYPE_TEST:
      return (t->type << 24) + t->data.referent->common.hash_id;
    default:
    { char msg[BUFFER_MSG_SIZE];
    strncpy (msg, "production.c: Error: bad test type in hash_test\n", BUFFER_MSG_SIZE);
    msg[BUFFER_MSG_SIZE - 1] = 0; /* ensure null termination */
    abort_with_fatal_error(thisAgent, msg);
    break;
    }
  }
  return 0; /* unreachable, but without it, gcc -Wall warns here */
}

/* ----------------------------------------------------------------
   Returns TRUE iff the test contains an equality test for the given
   symbol.  If sym==NIL, returns TRUE iff the test contains any
   equality test.
---------------------------------------------------------------- */

Bool test_includes_equality_test_for_symbol (test t, Symbol *sym) {
  cons *c;

  if (test_is_blank(t)) return FALSE;

  if (t->type == EQUALITY_TEST) {
    if (sym) return (t->data.referent == sym);
    return TRUE;
  } else if (t->type==CONJUNCTIVE_TEST) {
    for (c=t->data.conjunct_list; c!=NIL; c=c->rest)
      if (test_includes_equality_test_for_symbol (static_cast<test>(c->first), sym)) return TRUE;
  }
  return FALSE;
}

/* ----------------------------------------------------------------
   Returns TRUE iff the test contains a test for a variable
   symbol.  Assumes test is not a conjunctive one and does not
   try to search them.
---------------------------------------------------------------- */
Bool test_is_variable(agent* thisAgent, test t)
{
  cons *c;
  char *this_test;
  bool return_value = false;

  if (test_is_blank(t)) return FALSE;
  if ((t->type == DISJUNCTION_TEST) ||
      (t->type == CONJUNCTIVE_TEST) ||
      (t->type == GOAL_ID_TEST) ||
      (t->type == IMPASSE_ID_TEST)) return FALSE;

  return (t->data.referent->common.symbol_type == VARIABLE_SYMBOL_TYPE);
}

/* ----------------------------------------------------------------
   Looks for goal or impasse tests (as directed by the two flag
   parameters) in the given test, and returns TRUE if one is found.
---------------------------------------------------------------- */

Bool test_includes_goal_or_impasse_id_test (test t,
                                            Bool look_for_goal,
                                            Bool look_for_impasse) {
  cons *c;

  if (t->type == EQUALITY_TEST) return FALSE;
  if (look_for_goal && (t->type==GOAL_ID_TEST)) return TRUE;
  if (look_for_impasse && (t->type==IMPASSE_ID_TEST)) return TRUE;
  if (t->type == CONJUNCTIVE_TEST) {
    for (c=t->data.conjunct_list; c!=NIL; c=c->rest)
      if (test_includes_goal_or_impasse_id_test (static_cast<test>(c->first),
                                                 look_for_goal,
                                                 look_for_impasse))
        return TRUE;
    return FALSE;
  }
  return FALSE;
}

/* ----------------------------------------------------------------
   Looks through a test, and returns a new copy of the first equality
   test it finds.  Signals an error if there is no equality test in
   the given test.
---------------------------------------------------------------- */

test copy_of_equality_test_found_in_test (agent* thisAgent, test t) {
  cons *c;
  char msg[BUFFER_MSG_SIZE];

  if (test_is_blank(t)) {
    strncpy (msg, "Internal error: can't find equality constraint in constraint\n", BUFFER_MSG_SIZE);
    msg[BUFFER_MSG_SIZE - 1] = 0; /* ensure null termination */
    abort_with_fatal_error(thisAgent, msg);
  }
  if (t->type == EQUALITY_TEST) return copy_test (thisAgent, t);
  if (t->type==CONJUNCTIVE_TEST) {
    for (c=t->data.conjunct_list; c!=NIL; c=c->rest)
      if ( (!test_is_blank (static_cast<test>(c->first))) &&
           (static_cast<test>(c->first)->type == EQUALITY_TEST) )
        return copy_test (thisAgent, static_cast<test>(c->first));
  }
  strncpy (msg, "Internal error: can't find equality constraint in constraint\n",BUFFER_MSG_SIZE);
  abort_with_fatal_error(thisAgent, msg);
  return 0; /* unreachable, but without it, gcc -Wall warns here */
}

/* =====================================================================

   Finding all variables from tests, conditions, and condition lists

   These routines collect all the variables in tests, etc.  Their
   "var_list" arguments should either be NIL or else should point to
   the header of the list of marked variables being constructed.
===================================================================== */

void add_all_variables_in_test (agent* thisAgent, test t,
                tc_number tc, list **var_list) {
  cons *c;
  Symbol *referent;

  if (test_is_blank(t)) return;

  switch (t->type) {
  case GOAL_ID_TEST:
  case IMPASSE_ID_TEST:
  case DISJUNCTION_TEST:
    break;
  case CONJUNCTIVE_TEST:
    for (c=t->data.conjunct_list; c!=NIL; c=c->rest)
      add_all_variables_in_test (thisAgent, static_cast<test>(c->first), tc, var_list);
    break;

  default:
    referent = t->data.referent;
    if (referent->common.symbol_type==VARIABLE_SYMBOL_TYPE)
      mark_variable_if_unmarked (thisAgent, referent, tc, var_list);
    break;
  }
}

void add_bound_variables_in_test (agent* thisAgent, test t,
                  tc_number tc, list **var_list) {
  cons *c;
  Symbol *referent;

  if (test_is_blank(t)) return;

  if (t->type == EQUALITY_TEST) {
    referent = t->data.referent;
    if (referent->common.symbol_type==VARIABLE_SYMBOL_TYPE)
      mark_variable_if_unmarked (thisAgent, referent, tc, var_list);
    return;
  } else if (t->type==CONJUNCTIVE_TEST) {
    for (c=t->data.conjunct_list; c!=NIL; c=c->rest)
      add_bound_variables_in_test (thisAgent, static_cast<test>(c->first), tc, var_list);
  }
}

/* -----------------------------------------------------------------
   Find first letter of test, or '*' if nothing appropriate.
   (See comments on first_letter_from_symbol for more explanation.)
----------------------------------------------------------------- */

char first_letter_from_test (test t) {
  cons *c;
  char ch;

  if (test_is_blank (t)) return '*';

  switch(t->type) {
    case EQUALITY_TEST: return first_letter_from_symbol (t->data.referent);
    case GOAL_ID_TEST: return 's';
    case IMPASSE_ID_TEST: return 'i';
    case CONJUNCTIVE_TEST:
      for (c=t->data.conjunct_list; c!=NIL; c=c->rest) {
        ch = first_letter_from_test (static_cast<test>(c->first));
        if (ch != '*') return ch;
      }
      return '*';
    default:  /* disjunction tests, and relational tests other than equality */
      return '*';
  }
}

/* ----------------------------------------------------------------------
                      Add Gensymmed Equality Test

   This routine destructively modifies a given test, adding to it a test
   for equality with a new gensym variable.
---------------------------------------------------------------------- */

void add_gensymmed_equality_test (agent* thisAgent, test *t, char first_letter) {
  Symbol *New;
  test eq_test;
  char prefix[2];

  prefix[0] = first_letter;
  prefix[1] = 0;
  New = generate_new_variable (thisAgent, prefix);
  eq_test = make_test (thisAgent, New, EQUALITY_TEST);
  symbol_remove_ref (thisAgent, New);
  add_new_test_to_test (thisAgent, t, eq_test);
}

void add_gensymmed_unique_equality_test (agent* thisAgent, test *t, char first_letter) {
  Symbol *New;
  test eq_test;
  char prefix[2];

  prefix[0] = first_letter;
  prefix[1] = 0;

  New = generate_new_variable (thisAgent, prefix);
  thisAgent->varname_table->make_varsym_unique(&New);
  print(thisAgent, "Debug | add_gensymmed_unique_equality_test just created unique symbol %s\n", New->var.name);

  eq_test = make_test (thisAgent, New, EQUALITY_TEST);
  // Debug | Do we really need this for original_tests since we clean them up?  Must make refcount cleanup more consistent
  //symbol_remove_ref (thisAgent, New);
  add_new_test_to_test (thisAgent, t, eq_test);
}

/* ----------------------------------------------------------------------
                      Add Rete Test List to Tests

   Given the additional Rete tests (besides the hashed equality test) at
   a certain node, we need to convert them into the equivalent tests in
   the conditions being reconstructed.  This procedure does this -- it
   destructively modifies the given currently-being-reconstructed-cond
   by adding any necessary extra tests to its three field tests.
---------------------------------------------------------------------- */

void add_rete_test_list_to_tests (agent* thisAgent,
                                  condition *cond, /* current cond */
                                  rete_test *rt) {
  Symbol *referent;
  test New;
  TestType test_type;

  // Initialize table
  for ( ; rt!=NIL; rt=rt->next) {

    if (rt->type==ID_IS_GOAL_RETE_TEST) {
      New = make_test(thisAgent, NIL, GOAL_ID_TEST);
    } else if (rt->type==ID_IS_IMPASSE_RETE_TEST) {
      New = make_test(thisAgent, NIL, IMPASSE_ID_TEST);
    } else if (rt->type==DISJUNCTION_RETE_TEST) {
      New = make_test(thisAgent, NIL, DISJUNCTION_TEST);
      New->data.disjunction_list = copy_symbol_list_adding_references (thisAgent, rt->data.disjunction_list);
    } else if (test_is_constant_relational_test(rt->type)) {
      test_type = relational_test_type_to_test_type(kind_of_relational_test(rt->type));
      referent = rt->data.constant_referent;
      New = make_test (thisAgent, referent, test_type);
    } else if (test_is_variable_relational_test(rt->type)) {
      test_type = relational_test_type_to_test_type(kind_of_relational_test(rt->type));
      if (! rt->data.variable_referent.levels_up) {
        /* --- before calling var_bound_in_reconstructed_conds, make sure
           there's an equality test in the referent location (add one if
           there isn't one already there), otherwise there'd be no variable
           there to test against --- */
        if (rt->data.variable_referent.field_num==0) {
          if (!test_includes_equality_test_for_symbol(cond->data.tests.id_test, NIL))
            add_gensymmed_equality_test (thisAgent, &(cond->data.tests.id_test), 's');
        } else if (rt->data.variable_referent.field_num==1) {
          if (!test_includes_equality_test_for_symbol(cond->data.tests.attr_test, NIL))
            add_gensymmed_equality_test (thisAgent, &(cond->data.tests.attr_test), 'a');
        } else {
          if (!test_includes_equality_test_for_symbol(cond->data.tests.value_test, NIL))
            add_gensymmed_equality_test (thisAgent, &(cond->data.tests.value_test), first_letter_from_test(cond->data.tests.attr_test));
        }
      }
      referent = var_bound_in_reconstructed_conds (thisAgent, cond,
          rt->data.variable_referent.field_num,
          rt->data.variable_referent.levels_up);
      New = make_test (thisAgent, referent, test_type);
    } else {
      char msg[BUFFER_MSG_SIZE];
      strncpy (msg, "Error: bad test_type in add_rete_test_to_test\n", BUFFER_MSG_SIZE);
      msg[BUFFER_MSG_SIZE - 1] = 0; /* ensure null termination */
      abort_with_fatal_error(thisAgent, msg);
      New = NIL; /* unreachable, but without it gcc -Wall warns here */
    }

    if (rt->right_field_num==0)
      add_new_test_to_test (thisAgent, &(cond->data.tests.id_test), New);
    else if (rt->right_field_num==2)
      add_new_test_to_test (thisAgent, &(cond->data.tests.value_test), New);
    else
      add_new_test_to_test (thisAgent, &(cond->data.tests.attr_test), New);
  }
}


/* ----------------------------------------------------------------------
                      Add Hash Info to ID Test

   This routine adds an equality test to the id field test in a given
   condition, destructively modifying that id test.  The equality test
   is the one appropriate for the given hash location (field_num/levels_up).
---------------------------------------------------------------------- */

void add_hash_info_to_id_test (agent* thisAgent,
                               condition *cond,
                               byte field_num,
                               rete_node_level levels_up) {
  Symbol *temp;
  test New;

  temp = var_bound_in_reconstructed_conds (thisAgent, cond, field_num, levels_up);
  New = make_test (thisAgent, temp, EQUALITY_TEST);
  add_new_test_to_test (thisAgent, &(cond->data.tests.id_test), New);
}

/* ----------------------------------------------------------------------
                      Add Hash Info to ID Test

   This routine adds an equality test to the id field test in a given
   condition, destructively modifying that id test.  The equality test
   is the one appropriate for the given hash location (field_num/levels_up).
---------------------------------------------------------------------- */

void add_hash_info_to_original_id_test (agent* thisAgent,
                               condition *cond,
                               byte field_num,
                               rete_node_level levels_up) {
  Symbol *temp;
  test New;

  temp = var_bound_in_reconstructed_original_conds (thisAgent, cond, field_num, levels_up);
  thisAgent->varname_table->make_varsym_unique(&temp);
  print(thisAgent, "Debug | add_hash_info_to_original_id_test just created unique symbol %s.\n",
        temp->var.name);
  New = make_test (thisAgent, temp, EQUALITY_TEST);
  add_new_test_to_test (thisAgent, &(cond->data.tests.id_test->original_test), New);
}

/* ----------------------------------------------------------------------
                 add_additional_tests_and_originals

   This function gets passed the instantiated conditions for a production
   being fired.  It adds all the original tests in the given Rete test list
   (from the "other tests" at a Rete node), and adds them to the equality
   test in the instantiation. These tests will then also be variablized later.

   "Right_wme" is the wme that matched the current condition
   "cond" is the currently-being-reconstructed condition.

   - MMA 2013

---------------------------------------------------------------------- */

void add_additional_tests_and_originals (agent *thisAgent,
                                         rete_node *node,
                                         wme *right_wme,
                                         condition *cond,
                                         node_varnames *nvn)
{
  Symbol *referent, *original_referent, *right_sym;
  test chunk_test, original_test;
  TestType test_type;
  rete_test *rt = node->b.posneg.other_tests;

  /* --- store original referent information --- */

  alpha_mem *am;
  am = node->b.posneg.alpha_mem_;

  print(thisAgent, "\nDebug | add_additional_tests_and_originals called for %s.\n(%s ^%s %s)\n",
      thisAgent->newly_created_instantiations->prod->name->sc.name,
      (am->id ? symbol_to_string(thisAgent, am->id, NULL, NULL, 0) : "<blank>"),
      (am->attr ? symbol_to_string(thisAgent, am->attr, NULL, NULL, 0) : "<blank>"),
      (am->value ? symbol_to_string(thisAgent, am->value, NULL, NULL, 0) : "<blank>")
      );

  if (am->id && am->id->common.symbol_type == VARIABLE_SYMBOL_TYPE)
  {
    original_referent = am->id;
    print(thisAgent, "Debug | AATtTiC making am->id (%s) unique.\n",
          original_referent->var.name);
    thisAgent->varname_table->make_varsym_unique(&original_referent);
  } else {
    original_referent = am->id;
    if (am->id)
      print(thisAgent, "Debug | AATtTiC not making am->id (%s) unique.\n",
          symbol_to_string(thisAgent, original_referent, NULL, NULL, 0));
  }
  cond->data.tests.id_test->original_test = make_test(thisAgent, original_referent, EQUALITY_TEST);

  if (am->attr && am->attr->common.symbol_type == VARIABLE_SYMBOL_TYPE)
  {
    original_referent = am->attr;
    print(thisAgent, "Debug | AATtTiC making am->attr (%s) unique.\n",
          original_referent->var.name);
    thisAgent->varname_table->make_varsym_unique(&original_referent);
  } else {
    original_referent = am->attr;
    if (am->attr)
      print(thisAgent, "Debug | AATtTiC not making am->attr (%s) unique.\n",
          symbol_to_string(thisAgent, original_referent, NULL, NULL, 0));
  }
  cond->data.tests.attr_test->original_test = make_test(thisAgent, original_referent, EQUALITY_TEST);

  if (am->value && am->value->common.symbol_type == VARIABLE_SYMBOL_TYPE)
    {
      original_referent = am->value;
      print(thisAgent, "Debug | AATtTiC making am->value (%s) unique.\n",
          original_referent->var.name);
      thisAgent->varname_table->make_varsym_unique(&original_referent);
    } else {
      original_referent = am->value;
      if (am->value)
        print(thisAgent, "Debug | AATtTiC not making am->value (%s) unique.\n",
            symbol_to_string(thisAgent, original_referent, NULL, NULL, 0));
    }
  cond->data.tests.value_test->original_test = make_test(thisAgent, original_referent, EQUALITY_TEST);

  // Debug | Do we need to uniqueify here too?
  if (nvn) {
    print(thisAgent, "Debug | AATtTiC adding unique var names to original tests...\n");
    add_varnames_to_test (thisAgent, nvn->data.fields.id_varnames,
        &(cond->data.tests.id_test->original_test), true);
    add_varnames_to_test (thisAgent, nvn->data.fields.attr_varnames,
        &(cond->data.tests.attr_test->original_test), true);
    add_varnames_to_test (thisAgent, nvn->data.fields.value_varnames,
        &(cond->data.tests.value_test->original_test), true);
    print(thisAgent, "Debug | AATtTiC added unique var names to original tests resulting in:\n");
    print_test(thisAgent, cond->data.tests.id_test->original_test);
    print_test(thisAgent, cond->data.tests.attr_test->original_test);
    print_test(thisAgent, cond->data.tests.value_test->original_test);
  }

  /* --- on hashed nodes, add equality test for the hash function --- */
  if ((node->node_type==MP_BNODE) || (node->node_type==NEGATIVE_BNODE)) {
    print(thisAgent, "Debug | AATtTiC adding unique hash info to original id test...\n");
    add_hash_info_to_original_id_test (thisAgent, cond,
        node->left_hash_loc_field_num,
        node->left_hash_loc_levels_up);
    print(thisAgent, "Debug | AATtTiC added unique hash info to original id test resulting in:\n");
    print_test(thisAgent, cond->data.tests.id_test->original_test);
  } else if (node->node_type==POSITIVE_BNODE) {
    print(thisAgent, "Debug | AATtTiC adding unique hash info to original id test...\n");
    add_hash_info_to_original_id_test (thisAgent, cond,
        node->parent->left_hash_loc_field_num,
        node->parent->left_hash_loc_levels_up);
    print(thisAgent, "Debug | AATtTiC added unique hash info to original id test resulting in:\n");
    print_test(thisAgent, cond->data.tests.id_test->original_test);
  }

  for ( ; rt!=NIL; rt=rt->next) {

    /* Can probably skip entire loop if (a) one of three first test types or (b)
     * rt->right_field_num==0 (id field). Not needed for anything related to
     * chunking. Should probably also removed chunk_tests.id_test entirely.
     * Remove later after making sure not needed and we handle nil values. */

    /* If we want to remove adding goal and impasse tests later, we can  do
     * it here.  If those tests are also needed on instantiations (probably)
     * we'll need to make another similar function that does what this
     * function does but only to add goal and impasse tests (should be simple) */

    switch (rt->type) {
      case ID_IS_GOAL_RETE_TEST:
        // Do not create goal test in chunk test?
        chunk_test = NIL;
        original_test = make_test(thisAgent, NIL, GOAL_ID_TEST);
        break;
      case ID_IS_IMPASSE_RETE_TEST:
        // Do not create impasse test in chunk test?
        chunk_test = NIL;
        original_test = make_test(thisAgent, NIL, IMPASSE_ID_TEST);
        break;
      case DISJUNCTION_RETE_TEST:
        chunk_test = make_test(thisAgent, NIL, DISJUNCTION_TEST);
        chunk_test->data.disjunction_list = copy_symbol_list_adding_references (thisAgent, rt->data.disjunction_list);

        // Probably don't need to copy this disjunction list
        original_test->data.disjunction_list = copy_symbol_list_adding_references (thisAgent, rt->data.disjunction_list);

        break;
      default:
        if (test_is_constant_relational_test(rt->type))
        {
          test_type = relational_test_type_to_test_type(kind_of_relational_test(rt->type));
          referent = rt->data.constant_referent;
          chunk_test = make_test(thisAgent, referent, test_type);
          original_test = make_test (thisAgent, referent, test_type);
          // Debug | Can't I just do this?
          // original_test = chunk_test;
        }
        else if (test_is_variable_relational_test(rt->type))
        {
          test_type = relational_test_type_to_test_type(kind_of_relational_test(rt->type));
          if (!rt->data.variable_referent.levels_up)
          {
            /* --- before calling var_bound_in_reconstructed_conds, make sure
                   there's an equality test in the referent location (add one if
                   there isn't one already there), otherwise there'd be no variable
                   there to test against --- */
            switch (rt->data.variable_referent.field_num) {
              case 0:
                if (!test_includes_equality_test_for_symbol(cond->data.tests.id_test, NIL))
                {
                  print(thisAgent, "Debug | AATtTiC adding gensymmed but non-unique id test...\n");
                  add_gensymmed_equality_test (thisAgent, &(cond->data.tests.id_test), 's');
                  print(thisAgent, "Debug | AATtTiC added gensymmed but non-unique id test resulting in:\n");
                  print_test(thisAgent, cond->data.tests.id_test);
                }
                if (!test_includes_equality_test_for_symbol(cond->data.tests.id_test->original_test, NIL))
                {
                  print(thisAgent, "Debug | AATtTiC adding gensymmed but non-unique original id test...\n");
                  add_gensymmed_unique_equality_test (thisAgent, &(cond->data.tests.id_test->original_test), 's');
                  print(thisAgent, "Debug | AATtTiC added gensymmed but non-unique original id test resulting in:\n");
                  print_test(thisAgent, cond->data.tests.id_test->original_test);
                }
                break;
              case 1:
                if (!test_includes_equality_test_for_symbol(cond->data.tests.attr_test, NIL))
                {
                  print(thisAgent, "Debug | AATtTiC adding gensymmed but non-unique attr test...\n");
                  add_gensymmed_equality_test (thisAgent, &(cond->data.tests.attr_test), 'a');
                  print(thisAgent, "Debug | AATtTiC added gensymmed but non-unique attr test resulting in:\n");
                  print_test(thisAgent, cond->data.tests.attr_test);
                }
                break;
                if (!test_includes_equality_test_for_symbol(cond->data.tests.attr_test->original_test, NIL))
                 {
                  print(thisAgent, "Debug | AATtTiC adding gensymmed but non-unique original attr test...\n");
                  add_gensymmed_unique_equality_test (thisAgent, &(cond->data.tests.attr_test->original_test), 'a');
                  print(thisAgent, "Debug | AATtTiC added gensymmed but non-unique original attr test resulting in:\n");
                  print_test(thisAgent, cond->data.tests.attr_test->original_test);
                 }
                 break;
               default:
                if (!test_includes_equality_test_for_symbol(cond->data.tests.value_test, NIL))
                {
                  print(thisAgent, "Debug | AATtTiC adding gensymmed but non-unique value test...\n");
                  add_gensymmed_equality_test (thisAgent, &(cond->data.tests.value_test),
                      first_letter_from_test(cond->data.tests.attr_test));
                  print(thisAgent, "Debug | AATtTiC added gensymmed but non-unique value test resulting in:\n");
                  print_test(thisAgent, cond->data.tests.value_test);
                }
                if (!test_includes_equality_test_for_symbol(cond->data.tests.value_test->original_test, NIL))
                  {
                  print(thisAgent, "Debug | AATtTiC adding gensymmed but non-unique original value test...\n");
                  add_gensymmed_unique_equality_test (thisAgent, &(cond->data.tests.value_test->original_test),
                        first_letter_from_test(cond->data.tests.attr_test->original_test));
                  print(thisAgent, "Debug | AATtTiC added gensymmed but non-unique original value test resulting in:\n");
                  print_test(thisAgent, cond->data.tests.value_test->original_test);
                  }
                  break;
            }
          }

          referent = var_bound_in_reconstructed_conds (thisAgent, cond,
              rt->data.variable_referent.field_num,
              rt->data.variable_referent.levels_up);
          // Debug | Just to test...
          right_sym = field_from_wme (right_wme, rt->right_field_num);
          print(thisAgent, "Debug | AATtTiC right_sym is %s.\n",
              symbol_to_string(thisAgent, right_sym, NULL, NULL, 0));
          original_referent = var_bound_in_reconstructed_original_conds (thisAgent, cond,
              rt->data.variable_referent.field_num,
              rt->data.variable_referent.levels_up);

          chunk_test = make_test(thisAgent, referent, test_type);
          if (original_referent->common.symbol_type == VARIABLE_SYMBOL_TYPE)
          {
            print(thisAgent, "Debug | AATtTiC creating unique original relational referent for %s...\n",
                symbol_to_string(thisAgent, original_referent, NULL, NULL, 0));
            thisAgent->varname_table->make_varsym_unique(&original_referent);
            original_test = make_test (thisAgent, original_referent, test_type);
            print(thisAgent, "Debug | AATtTiC created unique original relational referent %s.\n",
                symbol_to_string(thisAgent, original_referent, NULL, NULL, 0));
          }
          else
          {

            original_test = make_test (thisAgent, original_referent, test_type);
          }
        }
        else
        {
          print(thisAgent, "Debug | Bad test_type in collect_chunk_test_info.\n");
          assert(false);
          /* unreachable, but without it gcc -Wall warns here */
          chunk_test = NIL;
          original_test = NIL;
        }
        if (rt->right_field_num==0)
        {
          add_new_test_to_test (thisAgent, &(cond->data.tests.id_test), chunk_test, original_test);
          print(thisAgent, "Debug | AATtTiC adding relational test to id resulting in:\n");
          print_test(thisAgent, original_test);
        }
        else if (rt->right_field_num==2)
        {
          add_new_test_to_test (thisAgent, &(cond->data.tests.value_test), chunk_test, original_test);
          print(thisAgent, "Debug | AATtTiC adding relational test to value resulting in:\n");
          print_test(thisAgent, original_test);
        }
        else
        {
          add_new_test_to_test (thisAgent, &(cond->data.tests.attr_test), chunk_test, original_test);
          print(thisAgent, "Debug | AATtTiC adding relational test to attr resulting in:\n");
          print_test(thisAgent, original_test);
        }
        break;
    }
  }
  /* --- if we threw away the variable names in the original tests, make sure
   *     there's some equality test in each of the three fields --- */
  if (! nvn) {
    if (! test_includes_equality_test_for_symbol
        (cond->data.tests.id_test->original_test, NIL))
    {
      print(thisAgent, "Debug | AATtTiC adding gensymmed unique original id test bc no equality test...\n");
      add_gensymmed_unique_equality_test (thisAgent, &(cond->data.tests.id_test->original_test), 's');
      print(thisAgent, "Debug | AATtTiC added gensymmed unique original id test resulting in:\n");
      print_test(thisAgent, cond->data.tests.id_test->original_test);
    }
    if (! test_includes_equality_test_for_symbol
        (cond->data.tests.attr_test->original_test, NIL))
    {
      print(thisAgent, "Debug | AATtTiC adding gensymmed unique original attr test bc no equality test...\n");
      add_gensymmed_unique_equality_test (thisAgent, &(cond->data.tests.attr_test->original_test), 'a');
      print(thisAgent, "Debug | AATtTiC added gensymmed unique original attr test resulting in:\n");
      print_test(thisAgent, cond->data.tests.attr_test->original_test);
    }
    if (! test_includes_equality_test_for_symbol
        (cond->data.tests.value_test->original_test, NIL))
    {
      print(thisAgent, "Debug | AATtTiC adding gensymmed unique original value test bc no equality test...\n");
      add_gensymmed_unique_equality_test (thisAgent, &(cond->data.tests.value_test->original_test),
          first_letter_from_test (cond->data.tests.attr_test->original_test));
      print(thisAgent, "Debug | AATtTiC added gensymmed unique original value test resulting in:\n");
      print_test(thisAgent, cond->data.tests.value_test->original_test);
    }
  }
  print(thisAgent, "Debug | add_additional_tests_and_originals finished for %s.\n\n", thisAgent->newly_created_instantiations->prod->name->sc.name);

}