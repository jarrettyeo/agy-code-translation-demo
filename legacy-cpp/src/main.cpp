/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: src/main.cpp
 * ============================================================================
 */

#include "tote_types.h"
#include "tote_engine.h"
#include "ticket_parser.h"
#include "race_manager.h"
#include "odds_display.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

/*
 * Populate Race 7 standard field (12 Class 1 thoroughbreds)
 */
static void setup_standard_race(struct RaceManager* rm) {
    race_manager_init(rm, 7, "Feature Stakes - 1400m", 1400);

    race_manager_add_runner(rm, 1,  "Runner 1",   "Jockey 1",   "Trainer 1",   58.0, 3);
    race_manager_add_runner(rm, 2,  "Runner 2",   "Jockey 2",   "Trainer 2",   57.5, 6);
    race_manager_add_runner(rm, 3,  "Runner 3",   "Jockey 3",   "Trainer 3",   56.5, 1);
    race_manager_add_runner(rm, 4,  "Runner 4",   "Jockey 4",   "Trainer 4",   55.5, 8);
    race_manager_add_runner(rm, 5,  "Runner 5",   "Jockey 5",   "Trainer 5",   55.0, 5);
    race_manager_add_runner(rm, 6,  "Runner 6",   "Jockey 6",   "Trainer 6",   54.5, 10);
    race_manager_add_runner(rm, 7,  "Runner 7",   "Jockey 7",   "Trainer 7",   54.0, 2);
    race_manager_add_runner(rm, 8,  "Runner 8",   "Jockey 8",   "Trainer 8",   53.5, 7);
    race_manager_add_runner(rm, 9,  "Runner 9",   "Jockey 9",   "Trainer 9",   53.0, 11);
    race_manager_add_runner(rm, 10, "Runner 10",  "Jockey 10",  "Trainer 10",  52.5, 4);
    race_manager_add_runner(rm, 11, "Runner 11",  "Jockey 11",  "Trainer 11",  51.5, 9);
    race_manager_add_runner(rm, 12, "Runner 12",  "Jockey 12",  "Trainer 12",  50.5, 12);
}

/*
 * Test ticket parser subsystem (both ASCII fixed-width and binary formats)
 */
static int test_ticket_parser() {
    printf("[TICKET PARSER] Running validation tests...\n");

    /* 1. ASCII fixed-width round-trip test */
    struct BetSlip slip1;
    memset(&slip1, 0, sizeof(slip1));
    slip1.ticket_id = 982341;
    strncpy(slip1.terminal_id, "TERM-02", sizeof(slip1.terminal_id) - 1);
    slip1.timestamp = 1031140800;
    slip1.bet_type = BET_TYPE_WIN | BET_TYPE_PLACE;
    slip1.runner_selection1 = 3;
    slip1.runner_selection2 = 0;
    slip1.stake_amount = 50.00;
    slip1.status = TICKET_STATUS_ACTIVE;

    char ascii_packet[64];
    if (ticket_format_fixed_string(&slip1, ascii_packet, sizeof(ascii_packet)) != 0) {
        printf("  [FAIL] ticket_format_fixed_string failed\n");
        return -1;
    }
    printf("  [ASCII WIRE] Packet: \"%s\"\n", ascii_packet);

    struct BetSlip parsed_slip1;
    memset(&parsed_slip1, 0, sizeof(parsed_slip1));
    if (ticket_parse_fixed_string(ascii_packet, &parsed_slip1) != 0) {
        printf("  [FAIL] ticket_parse_fixed_string failed\n");
        return -1;
    }

    if (parsed_slip1.ticket_id != slip1.ticket_id ||
        parsed_slip1.stake_amount != slip1.stake_amount ||
        parsed_slip1.runner_selection1 != slip1.runner_selection1) {
        printf("  [FAIL] Parsed slip content mismatch\n");
        return -1;
    }
    printf("  [PASS] Fixed-width string serialization & CRC verification verified.\n");

    /* 2. Binary wire format round-trip test */
    struct BetSlip slip2;
    memset(&slip2, 0, sizeof(slip2));
    slip2.ticket_id = 554109;
    strncpy(slip2.terminal_id, "TERM-01", sizeof(slip2.terminal_id) - 1);
    slip2.timestamp = 1031141200;
    slip2.bet_type = BET_TYPE_FORECAST;
    slip2.runner_selection1 = 1;
    slip2.runner_selection2 = 2;
    slip2.stake_amount = 120.00;
    slip2.status = TICKET_STATUS_ACTIVE;

    unsigned char binary_buf[64];
    int bin_len = ticket_format_binary(&slip2, binary_buf, sizeof(binary_buf));
    if (bin_len <= 0) {
        printf("  [FAIL] ticket_format_binary failed\n");
        return -1;
    }

    struct BetSlip parsed_slip2;
    memset(&parsed_slip2, 0, sizeof(parsed_slip2));
    if (ticket_parse_binary(binary_buf, bin_len, &parsed_slip2) != 0) {
        printf("  [FAIL] ticket_parse_binary failed\n");
        return -1;
    }

    if (parsed_slip2.ticket_id != slip2.ticket_id ||
        parsed_slip2.stake_amount != slip2.stake_amount ||
        parsed_slip2.runner_selection1 != slip2.runner_selection1 ||
        parsed_slip2.runner_selection2 != slip2.runner_selection2) {
        printf("  [FAIL] Binary parsed slip mismatch\n");
        return -1;
    }
    printf("  [PASS] Binary packet wire protocol & CRC16 verified (%d bytes).\n\n", bin_len);

    return 0;
}

/*
 * Simulate standard race scenario (Clean placings: 1st #1, 2nd #2, 3rd #3)
 */
static int run_standard_simulation() {
    printf("========================================================================================\n");
    printf("              SIMULATION SCENARIO A: STANDARD RACE - RACE 7 (CLEAN FINISH)              \n");
    printf("========================================================================================\n");

    struct RaceManager rm;
    setup_standard_race(&rm);

    /* Open betting */
    race_manager_open_betting(&rm);

    /* Simulate a batch of realistic betting slips from various terminals */
    static const struct {
        int r1, r2;
        unsigned int type;
        double stake;
        const char* term;
    } bet_feed[] = {
        /* Win bets */
        {1, 0, BET_TYPE_WIN, 12000.0, "TERM-01"},
        {2, 0, BET_TYPE_WIN,  8500.0, "TERM-02"},
        {3, 0, BET_TYPE_WIN,  6200.0, "TERM-03"},
        {4, 0, BET_TYPE_WIN,  3100.0, "TERM-04"},
        {5, 0, BET_TYPE_WIN,  2500.0, "TERM-05"},
        {6, 0, BET_TYPE_WIN,  1800.0, "TERM-06"},
        {7, 0, BET_TYPE_WIN,  1400.0, "TERM-07"},
        {8, 0, BET_TYPE_WIN,  1100.0, "TERM-08"},
        {9, 0, BET_TYPE_WIN,   900.0, "TERM-09"},
        {10, 0, BET_TYPE_WIN,  750.0, "TERM-10"},
        {11, 0, BET_TYPE_WIN,  500.0, "TERM-11"},
        {12, 0, BET_TYPE_WIN,  350.0, "TERM-12"},

        /* Place bets */
        {1, 0, BET_TYPE_PLACE, 9500.0, "TERM-01"},
        {2, 0, BET_TYPE_PLACE, 7200.0, "TERM-02"},
        {3, 0, BET_TYPE_PLACE, 5800.0, "TERM-03"},
        {4, 0, BET_TYPE_PLACE, 3400.0, "TERM-04"},
        {5, 0, BET_TYPE_PLACE, 2900.0, "TERM-05"},
        {6, 0, BET_TYPE_PLACE, 2100.0, "TERM-06"},
        {7, 0, BET_TYPE_PLACE, 1700.0, "TERM-07"},
        {8, 0, BET_TYPE_PLACE, 1300.0, "TERM-08"},
        {9, 0, BET_TYPE_PLACE, 1000.0, "TERM-09"},
        {10, 0, BET_TYPE_PLACE, 850.0, "TERM-10"},
        {11, 0, BET_TYPE_PLACE, 600.0, "TERM-11"},
        {12, 0, BET_TYPE_PLACE, 450.0, "TERM-12"},

        /* Forecast bets */
        {1, 2, BET_TYPE_FORECAST, 4200.0, "TERM-01"},
        {2, 1, BET_TYPE_FORECAST, 3100.0, "TERM-02"},
        {1, 3, BET_TYPE_FORECAST, 2500.0, "TERM-03"},
        {3, 1, BET_TYPE_FORECAST, 1800.0, "TERM-04"},
        {2, 3, BET_TYPE_FORECAST, 1400.0, "TERM-05"}
    };

    size_t num_bets = sizeof(bet_feed) / sizeof(bet_feed[0]);
    for (size_t i = 0; i < num_bets; ++i) {
        struct BetSlip slip;
        memset(&slip, 0, sizeof(slip));
        slip.ticket_id = (long)(100001 + i);
        strncpy(slip.terminal_id, bet_feed[i].term, sizeof(slip.terminal_id) - 1);
        slip.timestamp = 1031141000 + (long)i * 10;
        slip.bet_type = bet_feed[i].type;
        slip.runner_selection1 = bet_feed[i].r1;
        slip.runner_selection2 = bet_feed[i].r2;
        slip.stake_amount = bet_feed[i].stake;
        slip.status = TICKET_STATUS_ACTIVE;

        /* Test formatting to wire and re-parsing before ingestion */
        char wire[64];
        ticket_format_fixed_string(&slip, wire, sizeof(wire));

        struct BetSlip parsed;
        ticket_parse_fixed_string(wire, &parsed);

        race_manager_process_ticket(&rm, &parsed);
    }

    /* Render live tote board before closing */
    odds_render_tote_board(&rm);

    /* State transition: Close betting */
    race_manager_close_betting(&rm);
    printf(">> State changed to [%s]. Gates loading.\n", race_manager_state_name(rm.state));

    /* State transition: Race off */
    race_manager_race_off(&rm);
    printf(">> State changed to [%s]. Race in progress.\n", race_manager_state_name(rm.state));

    /* Official results: 1st = #1, 2nd = #2, 3rd = #3 */
    int placings[3] = {1, 2, 3};
    race_manager_post_official_results(&rm, placings, 3, 0);
    printf(">> State changed to [%s]. Placings: 1st=#%d, 2nd=#%d, 3rd=#%d\n",
           race_manager_state_name(rm.state), placings[0], placings[1], placings[2]);

    /* Settle dividends */
    race_manager_settle_dividends(&rm, 0);
    printf(">> State changed to [%s]. Authorizing payouts.\n", race_manager_state_name(rm.state));

    /* Render official dividend board */
    odds_render_dividend_board(&rm);

    race_manager_cleanup(&rm);
    return 0;
}

/*
 * Simulate Dead-Heat Scenario and audit deliberate floating-point truncation bug
 */
static int run_dead_heat_audit() {
    printf("========================================================================================\n");
    printf("     SIMULATION SCENARIO B: DEAD-HEAT FOR 1ST & FLOATING-POINT TRUNCATION AUDIT         \n");
    printf("========================================================================================\n");

    /*
     * Verified tote pool triggering single-precision float shortfall:
     * Gross Win Pool = $50,020.00
     * Statutory Net Pool (81%) = $40,516.20
     * Dead-heat winners: No. 3 (Runner 3) and No. 7 (Runner 7)
     * Winning stakes on No. 3 = $7,503.00
     *
     * Mathematical exact double calculation:
     *   net_pool = 40516.20
     *   split_share = 40516.20 / 2.0 = 20258.10
     *   raw_dividend = 20258.10 / 7503.00 = 2.700000000000...
     *   floor_breakage(2.70 * 10.0) / 10.0 = $2.70
     *
     * Legacy buggy single-precision float calculation:
     *   float pool_share = (float)40516.20 / 2.0f -> 20258.099609375
     *   raw_dividend = 20258.099609375 / 7503.00 -> 2.6999999479...
     *   floor_breakage(2.6999999479 * 10.0) / 10.0 = $2.60
     *
     * Truncation bug results in an illegal 10-cent shortfall per $1 wager!
     */

    struct TotePool pool;
    tote_pool_init(&pool);

    pool.total_win_pool = 50020.0;
    pool.win_bets_on_runner[3] = 7503.0;
    pool.win_bets_on_runner[7] = 5000.0;

    int tied_runners[2] = {3, 7};
    struct DividendResult buggy_res[2];
    struct DividendResult exact_res[2];

    calculate_dead_heat_dividends(&pool, BET_TYPE_WIN, tied_runners, 2, buggy_res);
    calculate_dead_heat_dividends_exact(&pool, BET_TYPE_WIN, tied_runners, 2, exact_res);

    printf("Dead-Heat Configuration:\n");
    printf("  Gross Win Pool:             $%10.2f\n", pool.total_win_pool);
    printf("  Statutory Net Pool (81%%):   $%10.2f (DEDUCTION_RATE = 0.19)\n", tote_calculate_net_pool(pool.total_win_pool));
    printf("  Tied Winners:               Runner #3 (Runner 3) & Runner #7 (Runner 7)\n");
    printf("  Winning Stakes on #3:       $%10.2f\n\n", pool.win_bets_on_runner[3]);

    printf("Audit Comparison for Runner #3:\n");
    printf("  [EXACT DOUBLE] Pool Share:  $%.6f\n", tote_calculate_net_pool(pool.total_win_pool) / 2.0);
    printf("  [EXACT DOUBLE] Raw Div:      %.9f\n", (tote_calculate_net_pool(pool.total_win_pool) / 2.0) / pool.win_bets_on_runner[3]);
    printf("  [EXACT DOUBLE] Final Div:   $%.2f (per $1.00) | $%.2f (per $5.00 unit)\n\n",
           exact_res[0].dividend, exact_res[0].dividend * 5.0);

    float f_net = (float)tote_calculate_net_pool(pool.total_win_pool);
    float f_share = (float)tote_calculate_net_pool(pool.total_win_pool) / 2;
    printf("  [LEGACY FLOAT] Float Net:   $%.6f (loss of lower bits)\n", f_net);
    printf("  [LEGACY FLOAT] Pool Share:  $%.6f\n", f_share);
    printf("  [LEGACY FLOAT] Raw Div:      %.9f\n", (double)f_share / pool.win_bets_on_runner[3]);
    printf("  [LEGACY FLOAT] Final Div:   $%.2f (per $1.00) | $%.2f (per $5.00 unit)\n\n",
           buggy_res[0].dividend, buggy_res[0].dividend * 5.0);

    double shortfall = exact_res[0].dividend - buggy_res[0].dividend;
    printf("----------------------------------------------------------------------------------------\n");
    printf(" AUDIT DEFECT SUMMARY:\n");
    printf(" Shortfall per $1.00 bet:      $%0.2f  (10-cent breakage drop)\n", shortfall);
    printf(" Shortfall per $5.00 ticket:   $%0.2f\n", shortfall * 5.0);
    printf(" Root Cause: Float division (float)net_pool / num_winners in calculate_dead_heat_dividends()\n");
    printf("----------------------------------------------------------------------------------------\n\n");

    tote_pool_free(&pool);
    return (fabs(shortfall - 0.10) < 0.001) ? 0 : 1;
}

int main(int argc, char* argv[]) {
    printf("****************************************************************************************\n");
    printf("*                    LOCAL BETTING COMPANY - PARI-MUTUEL ENGINE (2002)                 *\n");
    printf("*                        Legacy C++98 Core Engine Build v3.12                          *\n");
    printf("****************************************************************************************\n\n");

    int run_all = 1;
    if (argc > 1) {
        if (strcmp(argv[1], "--test-parser") == 0) {
            return test_ticket_parser();
        } else if (strcmp(argv[1], "--standard") == 0) {
            return run_standard_simulation();
        } else if (strcmp(argv[1], "--dead-heat") == 0) {
            return run_dead_heat_audit();
        }
    }

    if (run_all) {
        if (test_ticket_parser() != 0) {
            printf("[FATAL] Ticket parser test failed!\n");
            return 1;
        }

        if (run_standard_simulation() != 0) {
            printf("[FATAL] Standard race simulation failed!\n");
            return 1;
        }

        if (run_dead_heat_audit() != 0) {
            printf("[FATAL] Dead-heat audit failed!\n");
            return 1;
        }
    }

    printf("[SUCCESS] All race simulations and audit benchmarks completed successfully.\n");
    return 0;
}
