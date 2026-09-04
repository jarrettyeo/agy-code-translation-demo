/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: src/tote_engine.cpp
 * ============================================================================
 */

#include "tote_engine.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define INITIAL_BET_CAPACITY 4096

/*
 * Initialize a new Tote pool structure with zeroed accumulators
 */
void tote_pool_init(struct TotePool* pool) {
    if (!pool) return;

    pool->total_win_pool = 0.0;
    pool->total_place_pool = 0.0;
    pool->total_forecast_pool = 0.0;

    memset(pool->win_bets_on_runner, 0, sizeof(pool->win_bets_on_runner));
    memset(pool->place_bets_on_runner, 0, sizeof(pool->place_bets_on_runner));
    memset(pool->forecast_bets, 0, sizeof(pool->forecast_bets));

    pool->bet_count = 0;
    pool->bet_capacity = INITIAL_BET_CAPACITY;
    pool->bets = (struct BetSlip*)malloc(sizeof(struct BetSlip) * pool->bet_capacity);
}

/*
 * Free dynamically allocated bet slips ledger
 */
void tote_pool_free(struct TotePool* pool) {
    if (!pool) return;
    if (pool->bets) {
        free(pool->bets);
        pool->bets = NULL;
    }
    pool->bet_count = 0;
    pool->bet_capacity = 0;
}

/*
 * Reset all pool accumulators but keep allocated capacity
 */
void tote_pool_reset(struct TotePool* pool) {
    if (!pool) return;
    pool->total_win_pool = 0.0;
    pool->total_place_pool = 0.0;
    pool->total_forecast_pool = 0.0;
    memset(pool->win_bets_on_runner, 0, sizeof(pool->win_bets_on_runner));
    memset(pool->place_bets_on_runner, 0, sizeof(pool->place_bets_on_runner));
    memset(pool->forecast_bets, 0, sizeof(pool->forecast_bets));
    pool->bet_count = 0;
}

/*
 * Accumulate a verified BetSlip into the tote pools
 */
int tote_pool_add_bet(struct TotePool* pool, const struct BetSlip* bet) {
    if (!pool || !bet) return -1;
    if (bet->status != TICKET_STATUS_ACTIVE) return -2;
    if (bet->stake_amount <= 0.0) return -3;

    /* Grow ledger buffer if needed */
    if (pool->bet_count >= pool->bet_capacity) {
        int new_capacity = pool->bet_capacity * 2;
        struct BetSlip* new_buf = (struct BetSlip*)realloc(pool->bets, sizeof(struct BetSlip) * new_capacity);
        if (!new_buf) return -4;
        pool->bets = new_buf;
        pool->bet_capacity = new_capacity;
    }

    /* Record in ledger */
    pool->bets[pool->bet_count++] = *bet;

    /* Update pool accumulators based on bitmask flags */
    if (bet->bet_type & BET_TYPE_WIN) {
        int r = bet->runner_selection1;
        if (r >= 1 && r < MAX_RUNNERS) {
            pool->total_win_pool += bet->stake_amount;
            pool->win_bets_on_runner[r] += bet->stake_amount;
        }
    }

    if (bet->bet_type & BET_TYPE_PLACE) {
        int r = bet->runner_selection1;
        if (r >= 1 && r < MAX_RUNNERS) {
            pool->total_place_pool += bet->stake_amount;
            pool->place_bets_on_runner[r] += bet->stake_amount;
        }
    }

    if (bet->bet_type & BET_TYPE_FORECAST) {
        int r1 = bet->runner_selection1;
        int r2 = bet->runner_selection2;
        if (r1 >= 1 && r1 < MAX_RUNNERS && r2 >= 1 && r2 < MAX_RUNNERS && r1 != r2) {
            pool->total_forecast_pool += bet->stake_amount;
            pool->forecast_bets[r1][r2] += bet->stake_amount;
        }
    }

    return 0;
}

/*
 * Calculate statutory net pool after government levy and club commission (19%)
 */
double tote_calculate_net_pool(double gross_pool) {
    if (gross_pool <= 0.0) return 0.0;
    return gross_pool * (1.0 - DEDUCTION_RATE);
}

/*
 * Apply statutory 10-cent downward floor breakage (Totalisator Wagering Rules)
 * Statutory formula: floor(dividend * 10.0) / 10.0
 */
double tote_apply_breakage(double raw_dividend) {
    if (raw_dividend <= 0.0) return 0.0;
    double broken = std::floor(raw_dividend * 10.0) / 10.0;
    if (broken < MINIMUM_DIVIDEND) {
        return MINIMUM_DIVIDEND;
    }
    return broken;
}

/*
 * Calculate Win Dividend
 */
struct DividendResult tote_calculate_win(const struct TotePool* pool, int winning_runner) {
    struct DividendResult res;
    memset(&res, 0, sizeof(res));
    res.bet_type = BET_TYPE_WIN;
    res.runner_number = winning_runner;
    res.gross_pool = pool ? pool->total_win_pool : 0.0;
    res.net_pool = tote_calculate_net_pool(res.gross_pool);
    res.is_dead_heat = 0;

    if (!pool || winning_runner < 1 || winning_runner >= MAX_RUNNERS) {
        return res;
    }

    res.winning_stakes = pool->win_bets_on_runner[winning_runner];
    if (res.winning_stakes > 0.0) {
        double raw = res.net_pool / res.winning_stakes;
        res.dividend = tote_apply_breakage(raw);
    } else {
        res.dividend = 0.0;
    }

    return res;
}

/*
 * Calculate Place Dividends (Standard 3 places)
 */
int tote_calculate_place(const struct TotePool* pool,
                        const int* placed_runners,
                        int num_places,
                        struct DividendResult* out_results) {
    if (!pool || !placed_runners || !out_results || num_places <= 0) {
        return -1;
    }

    double gross = pool->total_place_pool;
    double net = tote_calculate_net_pool(gross);
    double place_pool_share = net / (double)num_places;

    for (int i = 0; i < num_places; ++i) {
        int r = placed_runners[i];
        memset(&out_results[i], 0, sizeof(struct DividendResult));
        out_results[i].bet_type = BET_TYPE_PLACE;
        out_results[i].runner_number = r;
        out_results[i].gross_pool = gross;
        out_results[i].net_pool = net;
        out_results[i].is_dead_heat = 0;

        if (r >= 1 && r < MAX_RUNNERS) {
            out_results[i].winning_stakes = pool->place_bets_on_runner[r];
            if (out_results[i].winning_stakes > 0.0) {
                double raw = place_pool_share / out_results[i].winning_stakes;
                out_results[i].dividend = tote_apply_breakage(raw);
            } else {
                out_results[i].dividend = 0.0;
            }
        }
    }

    return num_places;
}

/*
 * Calculate Forecast Dividend (1st and 2nd in exact order)
 */
int tote_calculate_forecast(const struct TotePool* pool,
                           int first_runner,
                           int second_runner,
                           struct DividendResult* out_result) {
    if (!pool || !out_result) return -1;
    if (first_runner < 1 || first_runner >= MAX_RUNNERS) return -2;
    if (second_runner < 1 || second_runner >= MAX_RUNNERS) return -2;
    if (first_runner == second_runner) return -3;

    memset(out_result, 0, sizeof(struct DividendResult));
    out_result->bet_type = BET_TYPE_FORECAST;
    out_result->runner_number = first_runner;
    out_result->runner_number2 = second_runner;
    out_result->gross_pool = pool->total_forecast_pool;
    out_result->net_pool = tote_calculate_net_pool(out_result->gross_pool);
    out_result->winning_stakes = pool->forecast_bets[first_runner][second_runner];
    out_result->is_dead_heat = 0;

    if (out_result->winning_stakes > 0.0) {
        double raw = out_result->net_pool / out_result->winning_stakes;
        out_result->dividend = tote_apply_breakage(raw);
    } else {
        out_result->dividend = 0.0;
    }

    return 0;
}

/*
 * Dead-Heat Dividend Calculations with deliberate floating-point truncation bug
 *
 * HISTORICAL NOTE (Legacy 2002 Codebase):
 * Float division (float)net_pool / num_winners before rounding:
 * Single-precision float division truncates bits of precision.
 * For specific pools and stakes (e.g., $50,020 gross, $40,516.20 net, 2 winners, $7,503 stakes),
 * (float)40516.20f / 2 yields 20258.0996...
 * 20258.0996 / 7503.0 evaluates to 2.69999994...
 * floor(dividend * 10.0) / 10.0 drops down to $2.60 instead of statutory $2.70.
 * Result: 10-cent shortfall on dead-heat splits.
 */
int calculate_dead_heat_dividends(const struct TotePool* pool,
                                 unsigned int bet_type,
                                 const int* tied_runners,
                                 int num_winners,
                                 struct DividendResult* out_results) {
    if (!pool || !tied_runners || !out_results || num_winners <= 0) {
        return -1;
    }

    double gross_pool = 0.0;
    if (bet_type & BET_TYPE_WIN) {
        gross_pool = pool->total_win_pool;
    } else if (bet_type & BET_TYPE_PLACE) {
        gross_pool = pool->total_place_pool;
    }

    double net_pool = tote_calculate_net_pool(gross_pool);

    /* --- DELIBERATE LEGACY BUG START --- */
    /* Single-precision float division before rounding */
    float pool_share = (float)net_pool / num_winners;
    /* --- DELIBERATE LEGACY BUG END --- */

    for (int i = 0; i < num_winners; ++i) {
        int r = tied_runners[i];
        memset(&out_results[i], 0, sizeof(struct DividendResult));
        out_results[i].bet_type = bet_type;
        out_results[i].runner_number = r;
        out_results[i].gross_pool = gross_pool;
        out_results[i].net_pool = net_pool;
        out_results[i].is_dead_heat = 1;

        double winning_stakes = 0.0;
        if (bet_type & BET_TYPE_WIN) {
            winning_stakes = pool->win_bets_on_runner[r];
        } else if (bet_type & BET_TYPE_PLACE) {
            winning_stakes = pool->place_bets_on_runner[r];
        }

        out_results[i].winning_stakes = winning_stakes;

        if (winning_stakes > 0.0) {
            /* Raw dividend from float pool_share */
            double raw = (double)pool_share / winning_stakes;
            /* Statutory 10-cent floor breakage: floor(dividend * 10.0) / 10.0 */
            double broken = std::floor(raw * 10.0) / 10.0;
            if (broken < MINIMUM_DIVIDEND) broken = MINIMUM_DIVIDEND;
            out_results[i].dividend = broken;
        } else {
            out_results[i].dividend = 0.0;
        }
    }

    return num_winners;
}

/*
 * Mathematically correct IEEE-754 double precision Dead-Heat Dividend calculation
 */
int calculate_dead_heat_dividends_exact(const struct TotePool* pool,
                                       unsigned int bet_type,
                                       const int* tied_runners,
                                       int num_winners,
                                       struct DividendResult* out_results) {
    if (!pool || !tied_runners || !out_results || num_winners <= 0) {
        return -1;
    }

    double gross_pool = 0.0;
    if (bet_type & BET_TYPE_WIN) {
        gross_pool = pool->total_win_pool;
    } else if (bet_type & BET_TYPE_PLACE) {
        gross_pool = pool->total_place_pool;
    }

    double net_pool = tote_calculate_net_pool(gross_pool);
    double exact_share = net_pool / (double)num_winners;

    for (int i = 0; i < num_winners; ++i) {
        int r = tied_runners[i];
        memset(&out_results[i], 0, sizeof(struct DividendResult));
        out_results[i].bet_type = bet_type;
        out_results[i].runner_number = r;
        out_results[i].gross_pool = gross_pool;
        out_results[i].net_pool = net_pool;
        out_results[i].is_dead_heat = 1;

        double winning_stakes = 0.0;
        if (bet_type & BET_TYPE_WIN) {
            winning_stakes = pool->win_bets_on_runner[r];
        } else if (bet_type & BET_TYPE_PLACE) {
            winning_stakes = pool->place_bets_on_runner[r];
        }

        out_results[i].winning_stakes = winning_stakes;

        if (winning_stakes > 0.0) {
            double raw = exact_share / winning_stakes;
            double broken = std::floor((raw * 10.0) + 1e-9) / 10.0;
            if (broken < MINIMUM_DIVIDEND) broken = MINIMUM_DIVIDEND;
            out_results[i].dividend = broken;
        } else {
            out_results[i].dividend = 0.0;
        }
    }

    return num_winners;
}
