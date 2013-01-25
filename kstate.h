/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the KBUS State library.
 *
 * The Initial Developer of the Original Code is Kynesim, Cambridge UK.
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Kynesim, Cambridge UK
 *   Tony Ibbs <tibs@tonyibbs.co.uk>
 *
 * ***** END LICENSE BLOCK *****
 */

#ifndef _KSTATE_H_INCLUDED_
#define _KSTATE_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>   // for NAME_LEN

enum kstate_permissions {
  KSTATE_READ=1,          // The state may be read
  KSTATE_WRITE=2,         // The state may be written
};
typedef enum kstate_permissions kstate_permissions_t;

// The maximum length of a state name. We expect this to be 254
#define KSTATE_MAX_NAME_LEN   (NAME_MAX - 1)

struct kstate_state {
  char        *name;
  uint32_t     permissions;

  int          shm_fd;    // The file id we get back from shm_open
};
typedef struct kstate_state *kstate_state_p;

struct kstate_transaction {
  struct kstate_state       state;

};
typedef struct kstate_transaction *kstate_transaction_p;

// -------- TEXT AFTER THIS AUTOGENERATED - DO NOT EDIT --------
// Autogenerated by extract_hdrs.py on 2013-01-25 (Fri 25 Jan 2013) at 10:56

/*
 * Return true if the given state is subscribed.
 */
extern bool kstate_state_is_subscribed(kstate_state_p state);

/*
 * Return true if the given transaction is active
 */
extern bool kstate_transaction_is_active(kstate_transaction_p transaction);

/*
 * Return a state's name, or NULL if it is not subscribed.
 */
extern const char *kstate_get_state_name(kstate_state_p state);

/*
 * Return a transaction's state name, or NULL if it is not active.
 */
extern const char *kstate_get_transaction_state_name(kstate_transaction_p transaction);

/*
 * Return a state's permissions, or 0 if it is not subscribed.
 */
extern uint32_t kstate_get_state_permissions(kstate_state_p state);

/*
 * Return a transaction's state permissions, or 0 if it is not active.
 */
extern uint32_t kstate_get_transaction_state_permissions(kstate_transaction_p transaction);

/*
 * Print a representation of 'state' on output 'stream'.
 *
 * Assumes the state is valid.
 *
 * If 'start' is non-NULL, print it before the state (with no added whitespace).
 * If 'eol' is true, then print a newline after the state.
 */
extern void kstate_print_state(FILE           *stream,
                               char           *start,
                               kstate_state_p  state,
                               bool            eol);

/*
 * Print a representation of 'transaction' on output 'stream'.
 *
 * Assumes the transaction is valid.
 *
 * If 'start' is non-NULL, print it before the transaction (with no added
 * whitespace).
 * If 'eol' is true, then print a newline after the transaction.
 */
extern void kstate_print_transaction(FILE                 *stream,
                                     char                 *start,
                                     kstate_transaction_p  transaction,
                                     bool                  eol);

/*
 * Create a new "empty" state.
 *
 * The normal usage is to create an empty state and then immediately
 * populate it::
 *
 *     kstate_state_p state = kstate_new_state();
 *     int ret = kstate_subscribe("State.Name", KSTATE_READ, state);
 *
 * and then eventually to destroy it::
 *
 *     int ret = kstate_unsubscribe(state);
 *     if (ret) {
 *       // deal with the error
 *     }
 *     kstate_free_state(&state);
 *
 * After which it can safely be reused, if you wish.
 *
 * Returns the new state, or NULL if there was insufficient memory.
 */
extern kstate_state_p kstate_new_state(void);

/*
 * Destroy a state created with 'kstate_new_state'.
 *
 * If a NULL pointer is given, then it is ignored, otherwise the state is
 * freed and the pointer is set to NULL.
 */
extern void kstate_free_state(kstate_state_p *state);

/*
 * Subscribe to a state.
 *
 * - ``name`` is the name of the state to subscribe to.
 * - ``permissions`` is constructed by OR'ing the permission flags
 *   KSTATE_READ and/or KSTATE_WRITE. At least one of those must be given.
 * - ``state`` is the actual state identifier, as amended by this function.
 *
 * A state name may contain A-Z, a-z, 0-9 and the dot (.) character. It may not
 * start or end with a dot, and may not contain adjacent dots. It must contain
 * at least one character. Note that the name will be copied into 'state'.
 *
 * If this is the first subscription to the named state, then the shared
 * data for the state will be created.
 *
 * Returns 0 if the subscription succeeds, or a negative value if it fails.
 * The negative value will be ``-errno``, giving an indication of why the
 * function failed.
 */
extern int kstate_subscribe(kstate_state_p         state,
                            const char            *name,
                            kstate_permissions_t   permissions);

/*
 * Unsubscribe from a state.
 *
 * - ``state`` is the state from which to unsubscribe.
 *
 * After this, the content of the state datastructure will have been
 * unset/freed. Unsubscribing from this same state value again will have no
 * effect.
 *
 * Note that transactions using the state keep their own copy of the state
 * information, and are not affected by this function - i.e., the state can
 * still be accessed via any transactions that are still open on it.
 *
 * Returns 0 if the unsubscription succeeds, or a negative value if it fails.
 * The negative value will be ``-errno``, giving an indication of why the
 * function failed.
 */
extern void kstate_unsubscribe(kstate_state_p  state);

/*
 * Create a new "empty" transaction.
 *
 * The normal usage is to create an empty transaction and then immediately
 * populate it::
 *
 *     struct kstate_transaction *transaction = kstate_new_transaction();
 *     int ret = kstate_start_transaction(&transaction, state);
 *
 * and then eventually to destroy it::
 *
 *     int ret = kstate_unsubscribe(transaction);
 *     if (ret) {
 *       // deal with the error
 *     }
 *     kstate_free_transaction(&transaction);
 *
 * Returns the new transaction, or NULL if there was insufficient memory.
 */
extern struct kstate_transaction *kstate_new_transaction(void);

/*
 * Destroy a transaction created with 'kstate_new_transaction'.
 *
 * If the transaction is still in progress, it will be aborted.
 *
 * If a NULL pointer is given, then it is ignored, otherwise the transaction is
 * freed and the pointer is set to NULL.
 */
extern void kstate_free_transaction(kstate_transaction_p *transaction);

/*
 * Start a new transaction on a state.
 *
 * If 'transaction' is still active, this will fail.
 *
 * * 'transaction' is the transaction to start.
 * * 'state' is the state on which to start the transaction.
 *
 * Note that a copy of the necessary information from 'state' will be held
 * in 'transaction'.
 *
 * Returns 0 if starting the transaction succeeds, or a negative value if it
 * fails. The negative value will be ``-errno``, giving an indication of why
 * the function failed.
 */
extern int kstate_start_transaction(kstate_transaction_p  transaction,
                                    kstate_state_p        state);

/*
 * Abort a transaction.
 *
 * - ``transaction`` is the transaction to abort.
 *
 * After this, the content of the transaction datastructure will have been
 * unset/freed.
 *
 * It is not allowed to abort a transaction that has not been started.
 * In other words, you cannot abort a transaction before it has been started,
 * or after it has been aborted or committed.
 *
 * Returns 0 if the abort succeeds, or a negative value if it fails.
 * The negative value will be ``-errno``, giving an indication of why the
 * function failed.
 */
extern int kstate_abort_transaction(kstate_transaction_p  transaction);

/*
 * Commit a transaction.
 *
 * - ``transaction`` is the transaction to commit.
 *
 * After this, the content of the transaction datastructure will have been
 * unset/freed.
 *
 * It is not allowed to commit a transaction that has not been started.
 * In other words, you cannot commit a transaction before it has been started,
 * or after it has been aborted or committed.
 *
 * Returns 0 if the commit succeeds, or a negative value if it fails.
 * The negative value will be ``-errno``, giving an indication of why the
 * function failed.
 */
extern int kstate_commit_transaction(struct kstate_transaction  *transaction);
// -------- TEXT BEFORE THIS AUTOGENERATED - DO NOT EDIT --------

#ifdef __cplusplus
}
#endif

#endif /* _KSTATE_H_INCLUDED_ */

// vim: set tabstop=8 softtabstop=2 shiftwidth=2 expandtab:
//
// Local Variables:
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
