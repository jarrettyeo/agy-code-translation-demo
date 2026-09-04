/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: include/odds_display.h
 * ============================================================================
 */

#ifndef ODDS_DISPLAY_H
#define ODDS_DISPLAY_H

#include "tote_types.h"
#include "race_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Calculate indicative Win dividend / odds for a runner
 * (Expected payout per $1.00 stake based on current live pool)
 */
double odds_calculate_win(const struct TotePool* pool, int runner_num);

/*
 * Calculate indicative Place dividend / odds range (min / max estimated)
 */
void odds_calculate_place_range(const struct TotePool* pool,
                               int runner_num,
                               int num_places,
                               double* out_min_odds,
                               double* out_max_odds);

/*
 * ASCII Local Grandstand Tote Board Renderer
 */
void odds_render_tote_board(const struct RaceManager* rm);

/*
 * ASCII Official Result & Dividend Payout Board Renderer
 */
void odds_render_dividend_board(const struct RaceManager* rm);

#ifdef __cplusplus
}
#endif

#endif /* ODDS_DISPLAY_H */
