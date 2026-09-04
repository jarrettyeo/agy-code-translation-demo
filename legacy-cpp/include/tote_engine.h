/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: include/tote_engine.h
 * ============================================================================
 */

#ifndef TOTE_ENGINE_H
#define TOTE_ENGINE_H

#include "tote_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Statutory deduction rate: 19% government levy & club commission */
#define DEDUCTION_RATE 0.19

/* Statutory floor breakage: 10 cents */
#define BREAKAGE_STEP 10.0

/*
 * Pool Lifecycle and Ingestion
 */
void tote_pool_init(struct TotePool* pool);
void tote_pool_free(struct TotePool* pool);
int tote_pool_add_bet(struct TotePool* pool, const struct BetSlip* bet);
void tote_pool_reset(struct TotePool* pool);

/*
 * Breakage and Deduction Helpers
 */
double tote_calculate_net_pool(double gross_pool);
double tote_apply_breakage(double raw_dividend);

/*
 * Standard Dividend Calculations
 */
struct DividendResult tote_calculate_win(const struct TotePool* pool, int winning_runner);

int tote_calculate_place(const struct TotePool* pool,
                        const int* placed_runners,
                        int num_places,
                        struct DividendResult* out_results);

int tote_calculate_forecast(const struct TotePool* pool,
                           int first_runner,
                           int second_runner,
                           struct DividendResult* out_result);

/*
 * Dead-Heat Dividend Calculations
 *
 * NOTE: calculate_dead_heat_dividends contains the legacy 2002 floating-point
 * truncation bug (float cast of net_pool / num_winners), causing a 10-cent shortfall.
 *
 * calculate_dead_heat_dividends_exact computes the correct 64-bit IEEE-754 double result.
 */
int calculate_dead_heat_dividends(const struct TotePool* pool,
                                 unsigned int bet_type,
                                 const int* tied_runners,
                                 int num_winners,
                                 struct DividendResult* out_results);

int calculate_dead_heat_dividends_exact(const struct TotePool* pool,
                                       unsigned int bet_type,
                                       const int* tied_runners,
                                       int num_winners,
                                       struct DividendResult* out_results);

#ifdef __cplusplus
}
#endif

#endif /* TOTE_ENGINE_H */
