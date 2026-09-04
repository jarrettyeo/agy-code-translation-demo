/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: include/race_manager.h
 * ============================================================================
 */

#ifndef RACE_MANAGER_H
#define RACE_MANAGER_H

#include "tote_types.h"
#include "tote_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DIVIDENDS_SETTLED 32

/*
 * Race Event and Lifecycle Context
 */
struct RaceManager {
    int race_number;
    char race_name[64];
    char race_class[32];
    int distance_meters;
    char track_condition[32];

    RaceState state;

    int runner_count;
    struct RunnerDef runners[MAX_RUNNERS];

    struct TotePool pool;

    /* Official race outcome */
    int official_placings[MAX_PLACED_RUNNERS];
    int num_placings;
    int is_dead_heat;

    /* Settled dividends */
    struct DividendResult settled_dividends[MAX_DIVIDENDS_SETTLED];
    int settled_dividend_count;
};

/*
 * Race Lifecycle Operations
 */
void race_manager_init(struct RaceManager* rm, int race_num, const char* name, int distance);
void race_manager_cleanup(struct RaceManager* rm);

int race_manager_add_runner(struct RaceManager* rm,
                           int runner_num,
                           const char* name,
                           const char* jockey,
                           const char* trainer,
                           double weight,
                           int barrier);

int race_manager_scratch_runner(struct RaceManager* rm, int runner_num);

/* State transitions */
RaceState race_manager_get_state(const struct RaceManager* rm);
const char* race_manager_state_name(RaceState state);

int race_manager_open_betting(struct RaceManager* rm);
int race_manager_close_betting(struct RaceManager* rm);
int race_manager_race_off(struct RaceManager* rm);
int race_manager_post_official_results(struct RaceManager* rm,
                                     const int* placings,
                                     int num_placings,
                                     int is_dead_heat);
int race_manager_settle_dividends(struct RaceManager* rm, int use_legacy_float);

/* Ticket processing (enforces state constraints) */
int race_manager_process_ticket(struct RaceManager* rm, const struct BetSlip* slip);

#ifdef __cplusplus
}
#endif

#endif /* RACE_MANAGER_H */
