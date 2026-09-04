/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: include/tote_types.h
 * ============================================================================
 */

#ifndef TOTE_TYPES_H
#define TOTE_TYPES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum runners supported in legacy tote pool array */
#define MAX_RUNNERS         32
#define MAX_PLACED_RUNNERS   4

/* Fixed string buffer lengths */
#define RUNNER_NAME_LEN     32
#define JOCKEY_NAME_LEN     32
#define TRAINER_NAME_LEN    32
#define TERMINAL_ID_LEN     16

/* Statutory pari-mutuel deductions (Totalisator Board Rules) */
#define STATUTORY_DEDUCTION_RATE    0.19    /* 19% commission / government levy */
#define BREAKAGE_UNIT               0.10    /* 10-cent downward floor breakage */
#define MINIMUM_DIVIDEND            1.00    /* Guaranteed minimum payout per $1 */

/* Bet Type Bitmasks (Legacy 8-bit mask field) */
#define BET_TYPE_NONE       0x00
#define BET_TYPE_WIN        0x01
#define BET_TYPE_PLACE      0x02
#define BET_TYPE_FORECAST   0x04

/* Ticket Status Codes */
#define TICKET_STATUS_ACTIVE    0
#define TICKET_STATUS_CANCELLED 1
#define TICKET_STATUS_REFUNDED  2

/*
 * Race Lifecycle State Machine
 */
typedef enum {
    RACE_STATE_OPEN = 0,            /* Betting active, terminals accepting wagers */
    RACE_STATE_BETTING_CLOSED = 1,  /* Gates loading, terminals locked */
    RACE_STATE_RACE_OFF = 2,        /* Starters released, race in progress */
    RACE_STATE_OFFICIAL_RESULT = 3, /* Judges posted verified placings */
    RACE_STATE_SETTLED = 4          /* Dividends finalized, payouts authorized */
} RaceState;

/*
 * Runner / Horse Definition
 */
struct RunnerDef {
    int runner_number;              /* 1-indexed cloth number */
    char name[RUNNER_NAME_LEN];     /* Horse racing name */
    char jockey[JOCKEY_NAME_LEN];   /* Jockey name */
    char trainer[TRAINER_NAME_LEN]; /* Trainer name */
    double weight_kg;               /* Carried weight in kilograms */
    int barrier_draw;               /* Starting barrier position */
    int scratched;                  /* 0 = active, 1 = scratched/withdrawn */
};

/*
 * Wagering Bet Slip Record (Legacy fixed-size struct)
 */
struct BetSlip {
    long ticket_id;                     /* Serialized ticket transaction ID */
    char terminal_id[TERMINAL_ID_LEN];  /* Terminal / OTB outlet identifier */
    long timestamp;                     /* POSIX epoch timestamp */
    unsigned int bet_type;              /* Bitmask: WIN, PLACE, FORECAST */
    int runner_selection1;              /* Primary runner number (1..MAX_RUNNERS) */
    int runner_selection2;              /* Secondary runner number (Forecast 2nd) */
    double stake_amount;                /* Wager amount in currency units */
    unsigned short checksum;            /* 16-bit CRC / additive checksum */
    int status;                         /* Active, cancelled, or refunded */
};

/*
 * Totalisator Pool Accumulator
 */
struct TotePool {
    double total_win_pool;
    double total_place_pool;
    double total_forecast_pool;

    /* Per-runner stakes accumulation (index = runner number, 1-based) */
    double win_bets_on_runner[MAX_RUNNERS];
    double place_bets_on_runner[MAX_RUNNERS];

    /* Forecast matrix: [1st runner][2nd runner] */
    double forecast_bets[MAX_RUNNERS][MAX_RUNNERS];

    /* Heap-allocated ticket ledger (raw pointers as per C++98 spec) */
    int bet_count;
    int bet_capacity;
    struct BetSlip* bets;
};

/*
 * Calculated Dividend Result
 */
struct DividendResult {
    unsigned int bet_type;
    int runner_number;          /* 1st runner or winning runner */
    int runner_number2;         /* 2nd runner (used for Forecast) */
    double dividend;            /* Payout per $1.00 stake */
    double gross_pool;          /* Total pool before deduction */
    double net_pool;            /* Net distributable pool after 19% deduction */
    double winning_stakes;      /* Total winning stake backing this outcome */
    int is_dead_heat;           /* 1 if dead-heat split, 0 otherwise */
};

#ifdef __cplusplus
}
#endif

#endif /* TOTE_TYPES_H */
