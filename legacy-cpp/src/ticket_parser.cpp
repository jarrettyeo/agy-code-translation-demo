/*
 * ============================================================================
 * Local Betting Company (c) 2002
 * System: Legacy Totalisator Subsystem (C++98 Standard)
 * File: src/ticket_parser.cpp
 * ============================================================================
 */

#include "ticket_parser.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

/*
 * Standard CCITT-16 CRC lookup table (polynomial 0x1021)
 */
static const unsigned short crc16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

/*
 * Calculate CRC16 checksum over a binary buffer
 */
unsigned short ticket_calculate_crc16(const unsigned char* data, size_t len) {
    unsigned short crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc = (unsigned short)((crc << 8) ^ crc16_table[((crc >> 8) ^ data[i]) & 0xFF]);
    }
    return crc;
}

/*
 * Calculate 16-bit additive/folded checksum over ASCII string buffer
 */
unsigned short ticket_calculate_string_checksum(const char* str, size_t len) {
    unsigned short sum = 0xAA55;
    for (size_t i = 0; i < len; ++i) {
        sum = (unsigned short)((sum * 33) + (unsigned char)str[i]);
    }
    return sum;
}

/*
 * Parse 48-byte fixed-width ASCII ticket string
 * Format:
 *   [00..02] "TKT"
 *   [03..10] Ticket ID (8 hex digits)
 *   [11..22] Terminal ID (12 chars)
 *   [23..32] Timestamp (10 decimal digits)
 *   [33..34] Bet Type (2 hex digits)
 *   [35..36] Runner 1 (2 decimal digits)
 *   [37..38] Runner 2 (2 decimal digits)
 *   [39..44] Stake in cents (6 decimal digits)
 *   [45..48] Checksum (4 hex digits)
 */
int ticket_parse_fixed_string(const char* raw_str, struct BetSlip* out_slip) {
    if (!raw_str || !out_slip) return -1;
    if (strlen(raw_str) < FIXED_TICKET_STRING_LEN) return -2;

    /* Validate header prefix */
    if (strncmp(raw_str, "TKT", 3) != 0) return -3;

    /* Extract and verify checksum */
    char chk_buf[5];
    memcpy(chk_buf, raw_str + 44, 4);
    chk_buf[4] = '\0';
    unsigned int read_chk = 0;
    if (sscanf(chk_buf, "%04X", &read_chk) != 1) return -4;

    unsigned short calculated_chk = ticket_calculate_string_checksum(raw_str, 44);
    if ((unsigned short)read_chk != calculated_chk) {
        return -5; /* Checksum mismatch */
    }

    /* Extract ticket ID */
    char id_buf[9];
    memcpy(id_buf, raw_str + 3, 8);
    id_buf[8] = '\0';
    unsigned long tkt_id = 0;
    sscanf(id_buf, "%08lX", &tkt_id);
    out_slip->ticket_id = (long)tkt_id;

    /* Extract terminal ID */
    char term_buf[13];
    memcpy(term_buf, raw_str + 11, 12);
    term_buf[12] = '\0';
    /* Trim trailing spaces */
    for (int i = 11; i >= 0 && term_buf[i] == ' '; --i) {
        term_buf[i] = '\0';
    }
    strncpy(out_slip->terminal_id, term_buf, TERMINAL_ID_LEN - 1);
    out_slip->terminal_id[TERMINAL_ID_LEN - 1] = '\0';

    /* Extract timestamp */
    char time_buf[11];
    memcpy(time_buf, raw_str + 23, 10);
    time_buf[10] = '\0';
    out_slip->timestamp = atol(time_buf);

    /* Extract bet type */
    char type_buf[3];
    memcpy(type_buf, raw_str + 33, 2);
    type_buf[2] = '\0';
    unsigned int btype = 0;
    sscanf(type_buf, "%02X", &btype);
    out_slip->bet_type = btype;

    /* Extract runner selections */
    char r1_buf[3], r2_buf[3];
    memcpy(r1_buf, raw_str + 35, 2);
    r1_buf[2] = '\0';
    memcpy(r2_buf, raw_str + 37, 2);
    r2_buf[2] = '\0';
    out_slip->runner_selection1 = atoi(r1_buf);
    out_slip->runner_selection2 = atoi(r2_buf);

    /* Extract stake amount (in cents) */
    char stake_buf[7];
    memcpy(stake_buf, raw_str + 39, 6);
    stake_buf[6] = '\0';
    long stake_cents = atol(stake_buf);
    out_slip->stake_amount = (double)stake_cents / 100.0;

    out_slip->checksum = calculated_chk;
    out_slip->status = TICKET_STATUS_ACTIVE;

    return 0;
}

/*
 * Serialize BetSlip into 48-byte fixed-width ASCII ticket string
 */
int ticket_format_fixed_string(const struct BetSlip* slip, char* out_buf, size_t buf_len) {
    if (!slip || !out_buf || buf_len < FIXED_TICKET_STRING_LEN + 1) return -1;

    char term_padded[13];
    snprintf(term_padded, sizeof(term_padded), "%-12.12s", slip->terminal_id);

    long stake_cents = (long)(slip->stake_amount * 100.0 + 0.5);

    /* Format first 44 bytes */
    snprintf(out_buf, buf_len, "TKT%08lX%12.12s%010ld%02X%02d%02d%06ld",
             slip->ticket_id,
             term_padded,
             slip->timestamp,
             slip->bet_type & 0xFF,
             slip->runner_selection1,
             slip->runner_selection2,
             stake_cents);

    /* Calculate checksum over first 44 bytes */
    unsigned short chk = ticket_calculate_string_checksum(out_buf, 44);

    /* Append 4-character hex checksum */
    char chk_str[5];
    snprintf(chk_str, sizeof(chk_str), "%04X", chk);
    memcpy(out_buf + 44, chk_str, 4);
    out_buf[48] = '\0';

    return 0;
}

/*
 * Parse binary ticket packet (AMT-400 terminal wire protocol)
 */
int ticket_parse_binary(const unsigned char* raw_data, size_t len, struct BetSlip* out_slip) {
    if (!raw_data || !out_slip) return -1;
    if (len < sizeof(struct BinaryTicketPacket)) return -2;

    const struct BinaryTicketPacket* pkt = (const struct BinaryTicketPacket*)raw_data;

    /* Verify sync markers */
    if (pkt->sync_marker[0] != TICKET_SYNC_BYTE1 || pkt->sync_marker[1] != TICKET_SYNC_BYTE2) {
        return -3;
    }

    /* Verify CRC16 over packet payload (excluding trailing 2-byte checksum) */
    size_t payload_len = sizeof(struct BinaryTicketPacket) - sizeof(unsigned short);
    unsigned short calculated_crc = ticket_calculate_crc16(raw_data, payload_len);
    if (calculated_crc != pkt->checksum) {
        return -4; /* CRC failure */
    }

    out_slip->ticket_id = (long)pkt->ticket_id;
    strncpy(out_slip->terminal_id, pkt->terminal_id, 12);
    out_slip->terminal_id[12] = '\0';
    out_slip->timestamp = (long)pkt->timestamp;
    out_slip->bet_type = pkt->bet_type;
    out_slip->runner_selection1 = pkt->runner1;
    out_slip->runner_selection2 = pkt->runner2;
    out_slip->stake_amount = (double)pkt->stake_cents / 100.0;
    out_slip->checksum = pkt->checksum;
    out_slip->status = TICKET_STATUS_ACTIVE;

    return 0;
}

/*
 * Format BetSlip into binary ticket packet
 */
int ticket_format_binary(const struct BetSlip* slip, unsigned char* out_buf, size_t buf_len) {
    if (!slip || !out_buf || buf_len < sizeof(struct BinaryTicketPacket)) return -1;

    struct BinaryTicketPacket* pkt = (struct BinaryTicketPacket*)out_buf;
    pkt->sync_marker[0] = TICKET_SYNC_BYTE1;
    pkt->sync_marker[1] = TICKET_SYNC_BYTE2;
    pkt->ticket_id = (unsigned int)slip->ticket_id;
    memset(pkt->terminal_id, 0, sizeof(pkt->terminal_id));
    strncpy(pkt->terminal_id, slip->terminal_id, sizeof(pkt->terminal_id) - 1);
    pkt->timestamp = (unsigned int)slip->timestamp;
    pkt->bet_type = (unsigned char)(slip->bet_type & 0xFF);
    pkt->runner1 = (unsigned char)slip->runner_selection1;
    pkt->runner2 = (unsigned char)slip->runner_selection2;
    pkt->flags = 0;
    pkt->stake_cents = (unsigned int)(slip->stake_amount * 100.0 + 0.5);

    /* Compute CRC16 over packet payload */
    size_t payload_len = sizeof(struct BinaryTicketPacket) - sizeof(unsigned short);
    pkt->checksum = ticket_calculate_crc16(out_buf, payload_len);

    return (int)sizeof(struct BinaryTicketPacket);
}

/*
 * Extract terminal location and terminal number
 * Format: "TERM-<LOCATION>-<UNIT>" (e.g. "TERM-METRO-04", "TERM-DOWNTOWN-01")
 */
int ticket_parse_terminal_info(const char* terminal_id, char* out_location, int* out_unit_num) {
    if (!terminal_id || !out_location || !out_unit_num) return -1;

    char temp[32];
    strncpy(temp, terminal_id, 31);
    temp[31] = '\0';

    /* Parse standard prefix TERM- */
    char* first_dash = strchr(temp, '-');
    if (!first_dash) {
        strcpy(out_location, temp);
        *out_unit_num = 1;
        return 0;
    }

    char* second_dash = strchr(first_dash + 1, '-');
    if (second_dash) {
        *second_dash = '\0';
        strcpy(out_location, first_dash + 1);
        *out_unit_num = atoi(second_dash + 1);
    } else {
        strcpy(out_location, "MAIN");
        *out_unit_num = atoi(first_dash + 1);
    }

    return 0;
}
