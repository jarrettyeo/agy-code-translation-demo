/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: include/ticket_parser.h
 * ============================================================================
 */

#ifndef TICKET_PARSER_H
#define TICKET_PARSER_H

#include "tote_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-width string ticket packet size */
#define FIXED_TICKET_STRING_LEN 48

/* Binary packet sync markers */
#define TICKET_SYNC_BYTE1 0x54  /* 'T' */
#define TICKET_SYNC_BYTE2 0x4B  /* 'K' */

#pragma pack(push, 1)
/*
 * Binary wire format for legacy terminal communication (AMT-400 / OTB network)
 */
struct BinaryTicketPacket {
    unsigned char sync_marker[2];       /* 'T', 'K' */
    unsigned int ticket_id;             /* 32-bit ticket transaction ID */
    char terminal_id[12];               /* 12-byte fixed terminal identifier */
    unsigned int timestamp;             /* Unix epoch timestamp */
    unsigned char bet_type;             /* Bitmask: WIN, PLACE, FORECAST */
    unsigned char runner1;              /* Primary runner number */
    unsigned char runner2;              /* Secondary runner number (Forecast) */
    unsigned char flags;                /* Status / revision flags */
    unsigned int stake_cents;           /* Stake amount in cents */
    unsigned short checksum;            /* 16-bit CRC / additive checksum */
};
#pragma pack(pop)

/*
 * Checksum calculation routines
 */
unsigned short ticket_calculate_crc16(const unsigned char* data, size_t len);
unsigned short ticket_calculate_string_checksum(const char* str, size_t len);

/*
 * Fixed-width ASCII ticket parser / serializer
 * String format:
 *   [00..02] "TKT"
 *   [03..10] Ticket ID (8 hex digits)
 *   [11..22] Terminal ID (12 chars, space-padded)
 *   [23..32] Timestamp (10 decimal digits)
 *   [33..34] Bet Type (2 hex digits)
 *   [35..36] Runner 1 (2 decimal digits)
 *   [37..38] Runner 2 (2 decimal digits)
 *   [39..44] Stake in cents (6 decimal digits)
 *   [45..48] Checksum (4 hex digits)
 */
int ticket_parse_fixed_string(const char* raw_str, struct BetSlip* out_slip);
int ticket_format_fixed_string(const struct BetSlip* slip, char* out_buf, size_t buf_len);

/*
 * Binary ticket parser / serializer
 */
int ticket_parse_binary(const unsigned char* raw_data, size_t len, struct BetSlip* out_slip);
int ticket_format_binary(const struct BetSlip* slip, unsigned char* out_buf, size_t buf_len);

/*
 * Terminal ID validation and metadata extraction
 * Format: "TERM-<UNIT>" (e.g. "TERM-01", "TERM-04")
 */
int ticket_parse_terminal_info(const char* terminal_id, char* out_location, int* out_unit_num);

#ifdef __cplusplus
}
#endif

#endif /* TICKET_PARSER_H */
