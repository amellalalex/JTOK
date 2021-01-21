/**
 * @file fake_escape_character_in_key_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test key names with "fake" escape characters in them
 * ie: escape characters that are of the form \\b rather than "\b"
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 *
 */
#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static const char *valid_jsons[] = {
    "{\"key\\n\\n\\b\":[]}",       "{\"key\\n\\n\\b\": []}",
    "{\"key\\n\\n\\b\":\t[]}",     "{\"key\\n\\n\\b\":\n[]}",
    "{\"key\\n\\n\\b\": [] }",     "{\"key\\n\\n\\b\":\t[] }",
    "{\"key\\n\\n\\b\":\n[] }",    "{\"key\\n\\n\\b\": []\t}",
    "{\"key\\n\\n\\b\":\t[]\t}",   "{\"key\\n\\n\\b\":\n[]\t}",
    "{\"key\\n\\n\\b\": []\n}",    "{\"key\\n\\n\\b\":\t[]\n}",
    "{\"key\\n\\n\\b\":\n[]\n}",   "{\"key\\n\\n\\b\": [ ]}",
    "{\"key\\n\\n\\b\":\t[ ]}",    "{\"key\\n\\n\\b\":\n[ ]}",
    "{\"key\\n\\n\\b\": [ ] }",    "{\"key\\n\\n\\b\":\t[ ] }",
    "{\"key\\n\\n\\b\":\n[ ] }",   "{\"key\\n\\n\\b\": [ ]\t}",
    "{\"key\\n\\n\\b\":\t[ ]\t}",  "{\"key\\n\\n\\b\":\n[ ]\t}",
    "{\"key\\n\\n\\b\": [ ]\n}",   "{\"key\\n\\n\\b\":\t[ ]\n}",
    "{\"key\\n\\n\\b\":\n[ ]\n}",  "{\"key\\n\\n\\b\": [\t]}",
    "{\"key\\n\\n\\b\":\t[\t]}",   "{\"key\\n\\n\\b\":\n[\t]}",
    "{\"key\\n\\n\\b\": [\t] }",   "{\"key\\n\\n\\b\":\t[\t] }",
    "{\"key\\n\\n\\b\":\n[\t] }",  "{\"key\\n\\n\\b\": [\t]\t}",
    "{\"key\\n\\n\\b\":\t[\t]\t}", "{\"key\\n\\n\\b\":\n[\t]\t}",
    "{\"key\\n\\n\\b\": [\t]\n}",  "{\"key\\n\\n\\b\":\t[\t]\n}",
    "{\"key\\n\\n\\b\":\n[\t]\n}", "{\"key\\n\\n\\b\": [\n]}",
    "{\"key\\n\\n\\b\":\t[\n]}",   "{\"key\\n\\n\\b\":\n[\n]}",
    "{\"key\\n\\n\\b\": [\n] }",   "{\"key\\n\\n\\b\":\t[\n] }",
    "{\"key\\n\\n\\b\":\n[\n] }",  "{\"key\\n\\n\\b\": [\n]\t}",
    "{\"key\\n\\n\\b\":\t[\n]\t}", "{\"key\\n\\n\\b\":\n[\n]\t}",
    "{\"key\\n\\n\\b\": [\n]\n}",  "{\"key\\n\\n\\b\":\t[\n]\n}",
    "{\"key\\n\\n\\b\":\n[\n]\n}",
};

static jtok_tkn_t tokens[TOKEN_MAX];
int               main(void)
{
    unsigned long long i;
    unsigned long long max_i = sizeof(valid_jsons) / sizeof(*valid_jsons);
    for (i = 0; i < max_i; i++)
    {
        printf("\n%s ... ", valid_jsons[i]);
        JTOK_PARSE_STATUS_t status;
        status = jtok_parse(valid_jsons[i], tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("failed with status %d.\n", status);
            return 1;
        }
        else
        {
            printf("passed.\n");
            continue;
        }
    }
}