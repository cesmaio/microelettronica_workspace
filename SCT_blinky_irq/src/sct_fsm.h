#ifndef __SCT_FSM_H__
#define __SCT_FSM_H__

/* Generated by fzmparser version 2.3 --- DO NOT EDIT! */

#include "sct_user.h"

extern void sct_fsm_init (void);

/* macros for defining the mapping between IRQ and events */
#define SCT_IRQ_EVENT_SCT_IRQ (0)


/* Match register reload macro definitions */
#define reload_MATCH0(value) LPC_SCT->MATCHREL[0].U = value;

#endif
