#pragma once

#define SESSION_ID_DICTIONARY "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define SESSION_ID_DICTIONARY_STRLEN strlen(SESSION_ID_DICTIONARY)
#define SESSION_ID_STRLEN (size_t)16 // SESSION_ID_DICTIONARY_STRLEN ^ 16 possible combinations.

// The signature is encoded as hex string.
// Each byte uses 2 characters. 256 bits (SHA-256) = 32 bytes. 32 bytes * 2 = 64 bytes.
#define SIGNATURE_STRLEN (size_t)64

#define SESSION_ENCODING_VERSION (uint8_t)1

// The token version is used for future token compatibilities.
#define TOKEN_VERSION "v1"
#define TOKEN_VERSION_STRLEN strlen(TOKEN_VERSION)
// TOKEN_VERSION_STRLEN + 1 dot + SESSION_ID_STRLEN + 1 dot + SIGNATURE_STRLEN.
#define TOKEN_STRLEN (size_t)(TOKEN_VERSION_STRLEN + 1 + SESSION_ID_STRLEN + 1 + SIGNATURE_STRLEN)

#define PAYLOAD_NAME_MAX_STRLEN (size_t)200
#define PAYLOAD_DATA_MAX_STRLEN (size_t)1e6 * 8 // 8 MB.
