/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: src/race_manager.cpp
 * ============================================================================
 */

#include "race_manager.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void race_manager_init(struct RaceManager* rm, int race_num, const char* name, int distance) {
    if (!rm) return;
    memset(rm, 0, sizeof(struct RaceManager));

    rm->race_number = race_num;
    strncpy(rm->race_name, name ? name : "Feature Stakes - 1400m", sizeof(rm->race_name) - 1);
    strncpy(rm->race_class, "Class 1", sizeof(rm->race_class) - 1);
    rm->distance_meters = distance > 0 ? distance : 1400;
    strncpy(rm->track_condition, "Good 4 (Turf - Long Course)", sizeof(rm->track_condition) - 1);

    rm->state = RACE_STATE_OPEN;
    rm->runner_count = 0;
    rm->num_placings = 0;
    rm->is_dead_heat = 0;
    rm->settled_dividend_count = 0;

    tote_pool_init(&rm->pool);
}

void race_manager_cleanup(struct RaceManager* rm) {
    if (!rm) return;
    tote_pool_free(&rm->pool);
}

int race_manager_add_runner(struct RaceManager* rm,
                           int runner_num,
                           const char* name,
                           const char* jockey,
                           const char* trainer,
                           double weight,
                           int barrier) {
    if (!rm || runner_num < 1 || runner_num >= MAX_RUNNERS) return -1;

    struct RunnerDef* r = &rm->runners[runner_num];
    r->runner_number = runner_num;
    strncpy(r->name, name ? name : "Unnamed", sizeof(r->name) - 1);
    strncpy(r->jockey, jockey ? jockey : "Jockey", sizeof(r->jockey) - 1);
    strncpy(r->trainer, trainer ? trainer : "Trainer", sizeof(r->trainer) - 1);
    r->weight_kg = weight;
    r->barrier_draw = barrier;
    r->scratched = 0;

    if (runner_num > rm->runner_count) {
        rm->runner_count = runner_num;
    }
    return 0;
}

int race_manager_scratch_runner(struct RaceManager* rm, int runner_num) {
    if (!rm || runner_num < 1 || runner_num >= MAX_RUNNERS) return -1;
    rm->runners[runner_num].scratched = 1;
    return 0;
}

RaceState race_manager_get_state(const struct RaceManager* rm) {
    return rm ? rm->state : RACE_STATE_OPEN;
}

const char* race_manager_state_name(RaceState state) {
    switch (state) {
        case RACE_STATE_OPEN:            return "OPEN";
        case RACE_STATE_BETTING_CLOSED:  return "BETTING_CLOSED";
        case RACE_STATE_RACE_OFF:        return "RACE_OFF";
        case RACE_STATE_OFFICIAL_RESULT: return "OFFICIAL_RESULT";
        case RACE_STATE_SETTLED:         return "SETTLED";
        default:                         return "UNKNOWN";
    }
}

int race_manager_open_betting(struct RaceManager* rm) {
    if (!rm) return -1;
    rm->state = RACE_STATE_OPEN;
    return 0;
}

int race_manager_close_betting(struct RaceManager* rm) {
    if (!rm) return -1;
    if (rm->state != RACE_STATE_OPEN) {
        return -2; /* Illegal state transition */
    }
    rm->state = RACE_STATE_BETTING_CLOSED;
    return 0;
}

int race_manager_race_off(struct RaceManager* rm) {
    if (!rm) return -1;
    if (rm->state != RACE_STATE_BETTING_CLOSED) {
        return -2; /* Betting must be closed before race off */
    }
    rm->state = RACE_STATE_RACE_OFF;
    return 0;
}

int race_manager_post_official_results(struct RaceManager* rm,
                                     const int* placings,
                                     int num_placings,
                                     int is_dead_heat) {
    if (!rm || !placings || num_placings <= 0) return -1;
    if (rm->state != RACE_STATE_RACE_OFF) {
        return -2; /* Results can only be posted after race off */
    }

    rm->num_placings = num_placings > MAX_PLACED_RUNNERS ? MAX_PLACED_RUNNERS : num_placings;
    for (int i = 0; i < rm->num_placings; ++i) {
        rm->official_placings[i] = placings[i];
    }
    rm->is_dead_heat = is_dead_heat;
    rm->state = RACE_STATE_OFFICIAL_RESULT;
    return 0;
}

int race_manager_process_ticket(struct RaceManager* rm, const struct BetSlip* slip) {
    if (!rm || !slip) return -1;
    if (rm->state != RACE_STATE_OPEN) {
        return -2; /* Terminals locked: betting not open */
    }

    /* Validate runner not scratched */
    if (slip->runner_selection1 >= 1 && slip->runner_selection1 < MAX_RUNNERS) {
        if (rm->runners[slip->runner_selection1].scratched) {
            return -3; /* Bet rejected: scratched runner */
        }
    }
    if ((slip->bet_type & BET_TYPE_FORECAST) &&
        slip->runner_selection2 >= 1 && slip->runner_selection2 < MAX_RUNNERS) {
        if (rm->runners[slip->runner_selection2].scratched) {
            return -4; /* Forecast rejected: runner 2 scratched */
        }
    }

    return tote_pool_add_bet(&rm->pool, slip);
}

int race_manager_settle_dividends(struct RaceManager* rm, int use_legacy_float) {
    if (!rm) return -1;
    if (rm->state != RACE_STATE_OFFICIAL_RESULT) {
        return -2; /* Official results required before settlement */
    }

    rm->settled_dividend_count = 0;

    /* 1. Settle WIN pool */
    if (rm->is_dead_heat && rm->num_placings >= 2) {
        /* Dead-heat for 1st between placings[0] and placings[1] */
        int tied[2];
        tied[0] = rm->official_placings[0];
        tied[1] = rm->official_placings[1];

        struct DividendResult dh_res[2];
        if (use_legacy_float) {
            calculate_dead_heat_dividends(&rm->pool, BET_TYPE_WIN, tied, 2, dh_res);
        } else {
            calculate_dead_heat_dividends_exact(&rm->pool, BET_TYPE_WIN, tied, 2, dh_res);
        }

        rm->settled_dividends[rm->settled_dividend_count++] = dh_res[0];
        rm->settled_dividends[rm->settled_dividend_count++] = dh_res[1];
    } else if (rm->num_placings >= 1) {
        struct DividendResult win_res = tote_calculate_win(&rm->pool, rm->official_placings[0]);
        rm->settled_dividends[rm->settled_dividend_count++] = win_res;
    }

    /* 2. Settle PLACE pool (3 places) */
    if (rm->num_placings >= 3) {
        struct DividendResult place_res[3];
        tote_calculate_place(&rm->pool, rm->official_placings, 3, place_res);
        for (int i = 0; i < 3; ++i) {
            rm->settled_dividends[rm->settled_dividend_count++] = place_res[i];
        }
    }

    /* 3. Settle FORECAST pool */
    if (rm->num_placings >= 2) {
        struct DividendResult fcast_res;
        tote_calculate_forecast(&rm->pool, rm->official_placings[0], rm->official_placings[1], &fcast_res);
        rm->settled_dividends[rm->settled_dividend_count++] = fcast_res;
    }

    rm->state = RACE_STATE_SETTLED;
    return rm->settled_dividend_count;
}
