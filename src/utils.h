#pragma once

#include <string.h>

#include "sodium.h"

#include "consts.h"

void generatePseudoRandomString(char *generatedStr);
void signData(const unsigned char *key, const size_t keyLen, const unsigned char *data, const size_t dataLen, char *signature);
void parseToken(const char *token, char *tokenVersion, char *sessionId, char *signature);
