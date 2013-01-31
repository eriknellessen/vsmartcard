/*
 * Copyright (C) 2010 Frank Morgner
 *
 * This file is part of npa.
 *
 * npa is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * npa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * npa.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
*/
/**
 * @file
 * @defgroup npa Interface to German identity card (neuer Personalausweis, nPA)
 * @{
 */
#ifndef _CCID_NPA_H
#define _CCID_NPA_H

#include <eac/eac.h>
#include <eac/pace.h>
#include <libopensc/opensc.h>
#include <libopensc/pace.h>
#include <npa/sm.h>

#ifdef __cplusplus
extern "C" {
#endif

/** NPA capabilities (TR-03119): PACE */
#define NPA_BITMAP_PACE  0x40
/** NPA capabilities (TR-03119): EPA: eID */
#define NPA_BITMAP_EID   0x20
/** NPA capabilities (TR-03119): EPA: eSign */
#define NPA_BITMAP_ESIGN 0x10

/** NPA result (TR-03119): Kein Fehler */
#define NPA_SUCCESS                            0x00000000
/** NPA result (TR-03119): Längen im Input sind inkonsistent */
#define NPA_ERROR_LENGTH_INCONSISTENT          0xD0000001
/** NPA result (TR-03119): Unerwartete Daten im Input */
#define NPA_ERROR_UNEXPECTED_DATA              0xD0000002
/** NPA result (TR-03119): Unerwartete Kombination von Daten im Input */
#define NPA_ERROR_UNEXPECTED_DATA_COMBINATION  0xD0000003
/** NPA result (TR-03119): Die Karte unterstützt das PACE – Verfahren nicht.  (Unerwartete Struktur in Antwortdaten der Karte) */
#define NPA_ERROR_CARD_NOT_SUPPORTED           0xE0000001
/** NPA result (TR-03119): Der Kartenleser unterstützt den angeforderten bzw. den ermittelten Algorithmus nicht.  */
#define NPA_ERROR_ALGORITH_NOT_SUPPORTED       0xE0000002
/** NPA result (TR-03119): Der Kartenleser kennt die PIN – ID nicht. */
#define NPA_ERROR_PINID_NOT_SUPPORTED          0xE0000003
/** NPA result (TR-03119): Negative Antwort der Karte auf Select EF_CardAccess (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_SELECT_EF_CARDACCESS         0xF0000000
/** NPA result (TR-03119): Negative Antwort der Karte auf Read Binary (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_READ_BINARY                  0xF0010000
/** NPA result (TR-03119): Negative Antwort der Karte auf MSE: Set AT (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_MSE_SET_AT                   0xF0020000
/** NPA result (TR-03119): Negative Antwort der Karte auf General Authenticate Step 1 (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_GENERAL_AUTHENTICATE_1       0xF0030000
/** NPA result (TR-03119): Negative Antwort der Karte auf General Authenticate Step 2 (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_GENERAL_AUTHENTICATE_2       0xF0040000
/** NPA result (TR-03119): Negative Antwort der Karte auf General Authenticate Step 3 (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_GENERAL_AUTHENTICATE_3       0xF0050000
/** NPA result (TR-03119): Negative Antwort der Karte auf General Authenticate Step 4 (needs to be OR-ed with SW1|SW2) */
#define NPA_ERROR_GENERAL_AUTHENTICATE_4       0xF0060000
/** NPA result (TR-03119): Kommunikationsabbruch mit Karte. */
#define NPA_ERROR_COMMUNICATION                0xF0100001
/** NPA result (TR-03119): Keine Karte im Feld. */
#define NPA_ERROR_NO_CARD                      0xF0100002
/** NPA result (TR-03119): Benutzerabbruch. */
#define NPA_ERROR_ABORTED                      0xF0200001
/** NPA result (TR-03119): Benutzer – Timeout */
#define NPA_ERROR_TIMEOUT                      0xF0200002

/** File identifier of EF.CardAccess */
#define  FID_EF_CARDACCESS   0x011C
/** Short file identifier of EF.CardAccess */
#define SFID_EF_CARDACCESS   0x1C
/** File identifier of EF.CardSecurity */
#define  FID_EF_CARDSECURITY 0x011D
/** Short file identifier of EF.CardAccess */
#define SFID_EF_CARDSECURITY 0x1D

/** Maximum length of PIN */
#define MAX_PIN_LEN       6
/** Minimum length of PIN */
#define MIN_PIN_LEN       6
/** Minimum length of MRZ */
#define MAX_MRZ_LEN       128

/**
 * @brief Names the type of the PACE secret
 *
 * @param pin_id type of the PACE secret
 *
 * @return Printable string containing the name
 */
const char *npa_secret_name(enum s_type pin_id);


/** 
 * @brief Get the PACE capabilities
 * 
 * @param[in,out] bitmap where to store capabilities bitmap
 * @note Since this code offers no support for terminal certificate, the bitmap is always \c PACE_BITMAP_PACE|PACE_BITMAP_EID
 * 
 * @return \c SC_SUCCESS or error code if an error occurred
 */
int get_pace_capabilities(u8 *bitmap);

/** 
 * @brief Establish secure messaging using PACE
 *
 * Modifies \a card to use the ISO SM driver and initializes the data
 * structures to use the established SM channel.
 *
 * Prints certificate description and card holder authorization template if
 * given in a human readable form to stdout. If no secret is given, the user is
 * asked for it. Only \a pace_input.pin_id is mandatory, the other members of
 * \a pace_input can be set to \c 0 or \c NULL respectively.
 *
 * The buffers in \a pace_output are allocated using \c realloc() and should be
 * set to NULL, if empty. If an EF.CardAccess is already present, this file is
 * reused and not fetched from the card.
 * 
 * @param[in,out] card
 * @param[in]     pace_input
 * @param[in,out] pace_output
 * @param[in]     tr_version  Version of TR-03110 to use with PACE
 * 
 * @return \c SC_SUCCESS or error code if an error occurred
 */
int perform_pace(sc_card_t *card,
        struct establish_pace_channel_input pace_input,
        struct establish_pace_channel_output *pace_output,
        enum eac_tr_version tr_version);

/**
 * @brief Terminal Authentication version 2
 *
 * @param[in] card
 * @param[in] certs              chain of cv certificates, the last certificate
 *                               is the terminal's certificate, array should be
 *                               terminated with \c NULL
 * @param[in] certs_lens         length of each element in \c certs, should be
 *                               terminated with \c 0
 * @param[in] privkey            The terminal's private key
 * @param[in] privkey_len        length of \a privkey
 * @param[in] auxiliary_data     auxiliary data for age/validity/community ID
 *                               verification. Should be formatted as buffer of
 *                               \c CVC_DISCRETIONARY_DATA_TEMPLATES
 * @param[in] auxiliary_data_len length of \a auxiliary_data
 *
 * @return \c SC_SUCCESS or error code if an error occurred
 */
int perform_terminal_authentication(sc_card_t *card,
        const unsigned char **certs, const size_t *certs_lens,
        const unsigned char *privkey, size_t privkey_len,
        const unsigned char *auxiliary_data, size_t auxiliary_data_len);

/**
 * @brief Establish secure messaging using Chip Authentication version 2
 *
 * Switches the SM context of \c card to the new established keys.
 *
 * @param[in] card
 *
 * @return \c SC_SUCCESS or error code if an error occurred
 */
int perform_chip_authentication(sc_card_t *card);

/** 
 * @brief Sends a reset retry counter APDU
 *
 * According to TR-03110 the reset retry counter APDU is used to set a new PIN
 * or to reset the retry counter of the PIN. The standard requires this
 * operation to be authorized either by an established PACE channel or by the
 * effective authorization of the terminal's certificate.
 * 
 * @param[in] card
 * @param[in] pin_id         Type of secret (usually PIN or CAN). You may use <tt>enum s_type</tt> from \c <openssl/pace.h>.
 * @param[in] ask_for_secret whether to ask the user for the secret (\c 1) or not (\c 0)
 * @param[in] new            (optional) new secret
 * @param[in] new_len        (optional) length of \a new
 * 
 * @return \c SC_SUCCESS or error code if an error occurred
 */
int npa_reset_retry_counter(sc_card_t *card,
        enum s_type pin_id, int ask_for_secret,
        const char *new, size_t new_len);
/** 
 * @brief Send APDU to unblock the PIN
 *
 * @param[in] card
 */
#define npa_unblock_pin(card) \
    npa_reset_retry_counter(card, PACE_PIN, 0, NULL, 0)
/** Send APDU to set a new PIN
 *
 * @param[in] card
 * @param[in] newp           (optional) new PIN
 * @param[in] newplen        (optional) length of \a new
 */
#define npa_change_pin(card, newp, newplen) \
    npa_reset_retry_counter(card, PACE_PIN, 1, newp, newplen)

/**
 * @brief Disable all sanity checks done by OpenPACE
 *
 * Currently used to set \c TA_FLAG_SKIP_TIMECHECK (Skip checking effective and
 * expiration date of cv certificates against the system's current time).
 */
#define NPA_FLAG_DISABLE_CHECKS 1

/** Use \c npa_default_flags to disable checks for EAC/SM */
extern char npa_default_flags;

#ifdef  __cplusplus
}
#endif
#endif
/* @} */
