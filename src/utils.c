#include "utils.h"

void generatePseudoRandomString(char *generatedStr) {
  uint8_t buf[SESSION_ID_STRLEN];
  randombytes_buf(buf, SESSION_ID_STRLEN);
  for (uint16_t i = 0; i < SESSION_ID_STRLEN; i++)
    generatedStr[i] =
        SESSION_ID_DICTIONARY[buf[i] % SESSION_ID_DICTIONARY_STRLEN];
}

void signData(const unsigned char *key, const size_t keyLen,
              const unsigned char *data, const size_t dataLen,
              char *signature) {
  unsigned char signatureHash[crypto_auth_hmacsha256_BYTES];
  crypto_auth_hmacsha256_state state;
  crypto_auth_hmacsha256_init(&state, key, keyLen);
  crypto_auth_hmacsha256_update(&state, data, dataLen);
  crypto_auth_hmacsha256_final(&state, signatureHash);
  sodium_bin2hex(signature, SIGNATURE_STRLEN + 1, signatureHash,
                 32); // 32 bytes = 256 bits.
}

void parseToken(const char *token, char *tokenVersion, char *sessionId,
                char *signature) {
  uint8_t i = 0;
  uint8_t ti = 0;
  for (i = 0; i < TOKEN_VERSION_STRLEN; i++, ti++)
    tokenVersion[i] = token[ti];
  tokenVersion[TOKEN_VERSION_STRLEN] = '\0';
  ti++;
  for (i = 0; i < SESSION_ID_STRLEN; i++, ti++)
    sessionId[i] = token[ti];
  sessionId[SESSION_ID_STRLEN] = '\0';
  ti++;
  for (i = 0; i < SIGNATURE_STRLEN; i++, ti++)
    signature[i] = token[ti];
  signature[SIGNATURE_STRLEN] = '\0';
}
