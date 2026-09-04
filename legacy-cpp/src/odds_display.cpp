/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: src/odds_display.cpp
 * ============================================================================
 */

#include "odds_display.h"
#include "tote_engine.h"
#include <cstdio>
#include <cmath>

double odds_calculate_win(const struct TotePool* pool, int runner_num) {
    if (!pool || runner_num < 1 || runner_num >= MAX_RUNNERS) return 0.0;
    double stakes = pool->win_bets_on_runner[runner_num];
    if (stakes <= 0.0) return 0.0;

    double net = tote_calculate_net_pool(pool->total_win_pool);
    return tote_apply_breakage(net / stakes);
}

void odds_calculate_place_range(const struct TotePool* pool,
                               int runner_num,
                               int num_places,
                               double* out_min_odds,
                               double* out_max_odds) {
    if (!out_min_odds || !out_max_odds) return;
    *out_min_odds = 0.0;
    *out_max_odds = 0.0;

    if (!pool || runner_num < 1 || runner_num >= MAX_RUNNERS || num_places <= 0) return;
    double stakes = pool->place_bets_on_runner[runner_num];
    if (stakes <= 0.0) return;

    double net = tote_calculate_net_pool(pool->total_place_pool);
    double share = net / (double)num_places;
    double est = tote_apply_breakage(share / stakes);

    *out_min_odds = est * 0.90;
    if (*out_min_odds < MINIMUM_DIVIDEND) *out_min_odds = MINIMUM_DIVIDEND;
    *out_max_odds = est * 1.10;
}

void odds_render_tote_board(const struct RaceManager* rm) {
    if (!rm) return;

    const struct TotePool* p = &rm->pool;
    double net_win = tote_calculate_net_pool(p->total_win_pool);
    double net_place = tote_calculate_net_pool(p->total_place_pool);
    double net_fcast = tote_calculate_net_pool(p->total_forecast_pool);
    double gross_total = p->total_win_pool + p->total_place_pool + p->total_forecast_pool;

    printf("\n");
    printf("========================================================================================\n");
    printf("                         LOCAL BETTING COMPANY - MAIN RACETRACK                         \n");
    printf("========================================================================================\n");
    printf(" Race %d: %-30s | Distance: %dm | Status: [%-14s]\n",
           rm->race_number, rm->race_name, rm->distance_meters, race_manager_state_name(rm->state));
    printf(" Track: %-26s | Class: %-12s | Tickets Processed: %d\n",
           rm->track_condition, rm->race_class, p->bet_count);
    printf("----------------------------------------------------------------------------------------\n");
    printf(" NO.  HORSE NAME          JOCKEY           WT(kg) DRW   WIN POOL($)  WIN($1)  PLACE POOL($)\n");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 1; i <= rm->runner_count; ++i) {
        const struct RunnerDef* r = &rm->runners[i];
        if (r->runner_number == 0) continue;

        if (r->scratched) {
            printf(" %02d   %-18.18s  %-15.15s  %5.1f  %3d   [SCRATCHED / WITHDRAWN]            \n",
                   r->runner_number, r->name, r->jockey, r->weight_kg, r->barrier_draw);
        } else {
            double w_pool = p->win_bets_on_runner[i];
            double w_odds = odds_calculate_win(p, i);
            double pl_pool = p->place_bets_on_runner[i];

            printf(" %02d   %-18.18s  %-15.15s  %5.1f  %3d   %11.2f  %7.2f  %13.2f\n",
                   r->runner_number, r->name, r->jockey, r->weight_kg, r->barrier_draw,
                   w_pool, w_odds, pl_pool);
        }
    }

    printf("----------------------------------------------------------------------------------------\n");
    printf(" TOTAL POOLS:  WIN: $%-11.2f | PLACE: $%-11.2f | FORECAST: $%-11.2f\n",
           p->total_win_pool, p->total_place_pool, p->total_forecast_pool);
    printf(" GROSS: $%-12.2f | COMM(19%%): $%-10.2f | NET POOL: $%-12.2f\n",
           gross_total, gross_total * STATUTORY_DEDUCTION_RATE,
           net_win + net_place + net_fcast);
    printf("========================================================================================\n\n");
}

void odds_render_dividend_board(const struct RaceManager* rm) {
    if (!rm) return;

    printf("\n");
    printf("========================================================================================\n");
    printf("                      OFFICIAL RACE DIVIDENDS & PAYOUT AUTHORIZATION                    \n");
    printf("========================================================================================\n");
    printf(" Race %d: %-30s | State: [%s]\n",
           rm->race_number, rm->race_name, race_manager_state_name(rm->state));

    if (rm->is_dead_heat) {
        printf(" NOTE: *** DEAD-HEAT FOR 1ST PLACE *** (Judges confirmed dead-heat finish)\n");
    }

    printf(" Official Placings: ");
    for (int i = 0; i < rm->num_placings; ++i) {
        printf("#%d ", rm->official_placings[i]);
    }
    printf("\n----------------------------------------------------------------------------------------\n");
    printf(" POOL TYPE   RUNNER(S)   WINNING STAKE($)   DIVIDEND ($1.00)   DIVIDEND ($5.00 UNIT)  DEAD HEAT\n");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < rm->settled_dividend_count; ++i) {
        const struct DividendResult* d = &rm->settled_dividends[i];
        const char* type_str = "UNKNOWN ";
        char runner_str[16];

        if (d->bet_type & BET_TYPE_WIN) {
            type_str = "WIN      ";
            snprintf(runner_str, sizeof(runner_str), "No. %02d", d->runner_number);
        } else if (d->bet_type & BET_TYPE_PLACE) {
            type_str = "PLACE    ";
            snprintf(runner_str, sizeof(runner_str), "No. %02d", d->runner_number);
        } else if (d->bet_type & BET_TYPE_FORECAST) {
            type_str = "FORECAST ";
            snprintf(runner_str, sizeof(runner_str), "%02d -> %02d", d->runner_number, d->runner_number2);
        }

        printf(" %-10s  %-10s  %16.2f   $%14.2f   $%19.2f  %-9s\n",
               type_str, runner_str, d->winning_stakes,
               d->dividend, d->dividend * 5.0,
               d->is_dead_heat ? "YES (BUGGY)" : "NO");
    }

    printf("========================================================================================\n\n");
}
