/**
 * @file null_param_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test API when null params are passed by caller
 * @version 0.1
 * @date 2021-01-12
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <stdio.h>
#include "jtok.h"

#define TOKEN_COUNT 5

static jtok_tkn_t tokens[TOKEN_COUNT];

int main(void)
{
    if (jtok_parse(NULL, tokens, TOKEN_COUNT) != JTOK_PARSE_STATUS_NULL_PARAM)
    {
        return 1;
    }

    if (jtok_parse("{\"key\" 1}", NULL, TOKEN_COUNT) != JTOK_PARSE_STATUS_NULL_PARAM)
    {
        return 1;
    }
    return 0;
}