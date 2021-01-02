/**
 * @file primitive_comparison_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test comparison functionality in JTOK
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 */
#include <stdio.h>


#include "jtok.h"


#define TOKEN_MAX 200

static struct
{
    char json1[250];
    char json2[250];
} true_cmp_table[] = {
    {.json1 = "{\"key\"  : 1 }", .json2 = "{\"key\":1}"},
    {.json1 = "{\"key\"  : 1 }", .json2 = "{\"key\":1 }"},
    {.json1 = "{\"key\"  : 1 }", .json2 = "{\"key\" : 1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" :+1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{\"key\":+1}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\": 1e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" :+1e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\": 1E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" :+1E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{\"key\":1.0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{\"key\":1.0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{\"key\" : 1 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1.0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" :+1.0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1.0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1.0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{\"key\":+1.0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\": 1.0e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1.0e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1.0e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1.0e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" :+1.0e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1.0e0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1.0e0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : 1.0E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\": 1.0E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1.0E0 }"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":1.0E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\":+1.0E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" :+1.0E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1.0E0}"},
    {.json1 = "{\"key\" : 1 }", .json2 = "{ \"key\" : +1.0E0 }"},
};


static jtok_tkn_t tokens1[TOKEN_MAX];
static jtok_tkn_t tokens2[TOKEN_MAX];

int main(void)
{
    unsigned long long i;
    unsigned long long max_i = sizeof(true_cmp_table) / sizeof(*true_cmp_table);
    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        bool passed = true;
        printf("\ncomparing %s and %s... ", true_cmp_table[i].json2,
               true_cmp_table[i].json1);

        status = jtok_parse(true_cmp_table[i].json1, tokens1, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            passed = false;
        }

        if (passed)
        {
            status = jtok_parse(true_cmp_table[i].json1, tokens2, TOKEN_MAX);
            if (status != JTOK_PARSE_STATUS_OK)
            {
                passed = false;
            }

            jtok_tkn_t *val1 = (jtok_tkn_t *)&tokens1[2];
            jtok_tkn_t *val2 = (jtok_tkn_t *)&tokens2[2];
            if (passed)
            {
                /* Compare "value" with "value" */
                if (!jtok_toktokcmp(val1, val2))
                {
                    passed = false;
                }
            }
        }


        if (passed)
        {
            printf("passed.\n");
            continue;
        }
        else
        {
            printf("failed.\n");
            return status;
        }
    }
    return 0;
}