/**
 * @file jtok.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Statically allocated JTOK parser for embedded systems
 * @version 0.4
 * @date 2020-11-13
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 */

#include "jtok.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define JTOK_ASCII_CHAR_LOWEST_VALUE 32   /* ' ' space */
#define JTOK_ASCII_CHAR_HIGHEST_VALUE 127 /* DEL */

#define INVALID_ARRAY_INDEX -1
#define NO_PARENT_IDX INVALID_ARRAY_INDEX
#define JTOK_STRING_INDEX_NONE INVALID_ARRAY_INDEX

#define HEXCHAR_ESCAPE_SEQ_COUNT 4 /* can escape 4 hex chars such as \uffea */


/**
 * @brief Allocate fresh token from the token pool
 *
 * @param parser
 * @param tokens
 * @param num_tokens
 * @return jtoktok_t*
 */
static jtoktok_t *jtok_alloc_token(jtok_parser_t *parser, jtoktok_t *tokens,
                                   size_t num_tokens);


/**
 * @brief Fill jtok_token type and boundaries
 *
 * @param token the jtok token to populate
 * @param type the token type
 * @param start stard index
 * @param end end index
 *
 * @return 0 on success, 1 on failure
 */
static int jtok_fill_token(jtoktok_t *token, jtoktype_t type, int start,
                           int end);


/**
 * @brief Parse and fill next available jtok token as a jtok string
 *
 * @param parser the jtok parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parse
 * @return JTOK_PARSE_STATUS_t parse status
 */
static JTOK_PARSE_STATUS_t
jtok_parse_string(jtok_parser_t *parser, jtoktok_t *tokens, size_t num_tokens);

/**
 * @brief Parse and fill the next available jtok token as a jtok array,
 * recursing into sub-objects
 *
 * @param parser the json parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parser
 * @return JTOK_PARSE_STATUS_t parser status
 */
static JTOK_PARSE_STATUS_t
jtok_parse_array(jtok_parser_t *parser, jtoktok_t *tokens, size_t num_tokens);


/**
 * @brief Parse and fill next available jtok token as a jtok primitive
 *
 * @param parser the json parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parse
 * @return JTOK_PARSE_STATUS_t parse status
 */
static JTOK_PARSE_STATUS_t jtok_parse_primitive(jtok_parser_t *parser,
                                                jtoktok_t *    tokens,
                                                size_t         num_tokens);


/**
 * @brief Parse and fill the next available jtok token as a jtok object,
 * recursing into sub-objects
 *
 * @param parser the json parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parser
 * @return JTOK_PARSE_STATUS_t parser status
 */
static JTOK_PARSE_STATUS_t
jtok_parse_object(jtok_parser_t *parser, jtoktok_t *tokens, size_t num_tokens);


/**
 * @brief Check if a parsed sequence of json tokens is valid
 *
 * @param parser the parser
 * @param tokens the token pool
 * @return JTOK_PARSE_STATUS_t status
 */
static JTOK_PARSE_STATUS_t tokenChainIsValid(const jtok_parser_t *parser,
                                             jtoktok_t *          tokens);


static jtoktok_t *jtok_alloc_token(jtok_parser_t *parser, jtoktok_t *tokens,
                                   size_t num_tokens);


char *jtok_toktypename(jtoktype_t type)
{
    static const char *jtoktok_type_names[] = {
        [JTOK_PRIMITIVE] = "JTOK_PRIMITIVE",
        [JTOK_OBJECT]    = "JTOK_OBJECT",
        [JTOK_ARRAY]     = "JTOK_ARRAY",
        [JTOK_STRING]    = "JTOK_STRING",
    };
    char *retval = NULL;
    switch (type)
    {
        case JTOK_PRIMITIVE:
        case JTOK_OBJECT:
        case JTOK_ARRAY:
        case JTOK_STRING:
        {
            retval = (char *)jtoktok_type_names[type];
        }
        break;
    }
    return retval;
}


char *jtok_jtokerr_messages(JTOK_PARSE_STATUS_t err)
{
    static const char *jtokerr_messages[] = {
        [0] = "Not enough jtoktok_t tokens were provided",
        [1] = "Invalid character inside JTOK string",
        [2] = "The string is not a full JTOK packet, more bytes expected",
    };
    char *retval = NULL;
    switch (err)
    {
        case JTOK_PARSE_STATUS_NOMEM:
        {
            retval = (char *)jtokerr_messages[0];
        }
        break;
        case JTOK_PARSE_STATUS_INVAL:
        {
            retval = (char *)jtokerr_messages[1];
        }
        break;
        case JTOK_PARSE_STATUS_PARTIAL_TOKEN:
        {
            retval = (char *)jtokerr_messages[2];
        }
        break;
    }
    return retval;
}

uint_least16_t jtok_toklen(const jtoktok_t *tok)
{
    uint_least16_t len = 0;
    if (tok != NULL)
    {
        uint_least64_t tokstart = tok->start;
        uint_least64_t tokend   = tok->end;
        if (tokend - tokstart < UINT16_MAX)
        {
            len = tokend - tokstart;
        }
    }
    return len;
}


#if defined(JTOK_STANDALONE_TOKENS)

bool jtok_tokcmp(const char *str, const jtoktok_t *tok)
{
    bool result = false;
    if (str == NULL)
    {
        if (tok != NULL && tok->json == NULL)
        {
            result = true;
        }
    }
    else if (tok != NULL && tok->json == NULL)
    {
        if (str == NULL)
        {
            result = true;
        }
    }
    else
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&tok->json[tok->start],
                    least_size) == 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

bool jtok_tokncmp(const char *str, const jtoktok_t *tok, uint_least16_t n)
{
    bool result = false;
    if (str != NULL && tok != NULL && tok->json != NULL)
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        if (least_size < n)
        {
            least_size = n;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&tok->json[tok->start],
                    least_size) == 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}


char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const jtoktok_t *tkn)
{
    char *result = NULL;
    if (dst != NULL && tkn != NULL && tkn->json != NULL)
    {
        uint_least16_t copy_count = jtok_toklen(tkn);
        if (copy_count > bufsize)
        {
            copy_count = bufsize;
        }
        result = strncpy(dst, (char *)&tkn->json[tkn->start], copy_count);
    }
    return result;
}

char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const jtoktok_t *tkn,
                   uint_least16_t n)
{
    char *         result = NULL;
    uint_least16_t count  = bufsize;
    if (bufsize > n)
    {
        count = n;
    }
    result = jtok_tokcpy(dst, count, tkn);
    return result;
}

#else /* JTOK_STANDALONE_TOKENS is not defined */


bool jtok_tokncmp(const uint8_t *str, const uint8_t *jtok, const jtoktok_t *tok,
                  uint_least16_t n)
{
    bool result = false;
    if (str != NULL && jtok != NULL && tok != NULL)
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        if (least_size < n)
        {
            least_size = n;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&jtok[tok->start], least_size) ==
            0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
}


bool jtok_tokcmp(const char *str, const uint8_t *jtok, const jtoktok_t *tok)
{
    bool result = false;
    if (str == NULL)
    {
        if (jtok == NULL)
        {
            result = true;
        }
        /* Fall to end, default return is false */
    }
    else if (jtok == NULL)
    {
        if (str == NULL)
        {
            result = true;
        }
        /* Fall to end, default return is false */
    }
    else
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&jtok[tok->start], least_size) ==
            0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const uint8_t *jtok,
                  const jtoktok_t *tkn)
{
    char *result = NULL;
    if (dst != NULL && jtok != NULL && tkn != NULL)
    {
        uint_least16_t copy_count = jtok_toklen(tkn);
        if (copy_count > bufsize)
        {
            copy_count = bufsize;
        }
        result = strncpy(dst, (char *)&jtok[tkn->start], copy_count);
    }
    return result;
}


char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const uint8_t *jtok,
                   const jtoktok_t *tkn, uint_least16_t n)
{
    char *         result = NULL;
    uint_least16_t count  = bufsize;
    if (bufsize > n)
    {
        count = n;
    }
    result = jtok_tokcpy(dst, count, jtok, tkn);
    return result;
}

#endif /*#if defined(JTOK_STANDALONE_TOKENS) */


bool isValidJson(const jtoktok_t *tokens, uint_least8_t tcnt)
{
    bool isValid = false;
    if (tokens != NULL)
    {
        if (tcnt > 1)
        {
            if (tokens[0].type == JTOK_OBJECT)
            {
                isValid = true;
            }

            if (tcnt == 2) /* Cannot have something like { "myKey" } */
            {
                if (tokens[1].type == JTOK_ARRAY)
                {
                    /* however, { [ ] } is still technically valid */
                    isValid = true;
                }
            }
            else
            {
                /* First key in a jtok must be a "string" */
                if (tcnt > 2)
                {
                    if (tokens[1].type == JTOK_STRING)
                    {
                        isValid = true;
                    }
                }
            }
        }
    }
    return isValid;
}


JTOK_PARSE_STATUS_t jtok_parse(jtok_parser_t *parser, jtoktok_t *tokens,
                               unsigned int num_tokens)
{
    /* Skip leading whitespace */
    char *json = parser->json;
    while (isspace(json[parser->pos]))
    {
        parser->pos++;
    }
    return jtok_parse_object(parser, tokens, num_tokens);
}


static int jtok_fill_token(jtoktok_t *token, jtoktype_t type, int start,
                           int end)
{
    if (token != NULL)
    {
        token->type  = type;
        token->start = start;
        token->end   = end;
        token->size  = 0;
        return 0;
    }
    else
    {
        return 1;
    }
}


static JTOK_PARSE_STATUS_t jtok_parse_primitive(jtok_parser_t *parser,
                                                jtoktok_t *    tokens,
                                                size_t         num_tokens)
{
    jtoktok_t *  token;
    int          start = parser->pos;
    const char * js    = (const char *)parser->json;
    unsigned int len   = parser->json_len;

    enum
    {
        NUMBER,
        BOOLEAN,
        ERROR
    } primitive_type = ERROR;

    bool exponent             = false;
    bool found_exponent_power = false;
    bool decimal              = false;
    bool found_decimal_places = false;
    for (start = parser->pos; parser->pos < len && js[parser->pos] != '\0';
         parser->pos++)
    {
        switch (js[parser->pos])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                if (parser->pos == start)
                {
                    primitive_type = NUMBER;
                }
                else
                {
                    if (primitive_type == NUMBER)
                    {
                        if (decimal)
                        {
                            found_decimal_places = true;
                        }
                    }
                    else
                    {
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
            }
            break;
            case '+':
            case '-':
                /* signs must come at beginning, or as an exponent */
                if (start == parser->pos)
                {
                    primitive_type = NUMBER;
                }
                else if (exponent)
                {
                    /* do nothing */
                }
                else
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }
                break;
            case '.': /* decimal */
            {
                if (parser->pos == start)
                {
                    /* {"key" : .123} is invalid */
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }
                else
                {
                    if (primitive_type == NUMBER)
                    {
                        if (decimal)
                        {
                            /* 123.0.1 is invalid primitive */
                            if (found_decimal_places)
                            {
                                return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                            }
                        }
                        else
                        {
                            decimal = true;
                        }

                        if (exponent)
                        {
                            /* { "key" : 123e+9.01} is invalid */
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                        }
                    }
                    else
                    {
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
            }
            break;
            case 'e':
            case 'E':
            {
                if (start == parser->pos)
                {
                    /* {"key" : e9"} is invalid */
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }
                else
                {
                    if (primitive_type == NUMBER)
                    {
                        /* previous char has to be a digit eg: 10e9 */
                        if (isdigit(js[parser->pos - 1]))
                        {
                            exponent = true;
                            continue;
                        }
                        else /* { "key" : -e9 } is invalid */
                        {
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                        }
                    }
                    else
                    {
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case ']':
            case '}':
            {
                if (tokens == NULL) /* caller provided no tokens at all */
                {
                    parser->pos--;
                    return JTOK_PARSE_STATUS_PARSE_OK;
                }
                char last = js[parser->pos - 1];

                if (exponent)
                {
                    switch (last)
                    {
                        case '+':
                        case '-':
                        case '.':
                        {
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                        }
                    }
                }

                if (decimal && last == '.')
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }

                token = jtok_alloc_token(parser, tokens, num_tokens);
                if (token == NULL) /* not enough tokens provided by caller */
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_NOMEM;
                }
                jtok_fill_token(token, JTOK_PRIMITIVE, start, parser->pos);
                token->parent = parser->toksuper;

                /* go back 1 spot so when we return from current function, the
                calling context can look at the current character */
                parser->pos--;
                return JTOK_PARSE_STATUS_PARSE_OK;
            }
            break;
            default:
            {
                if (parser->pos == start)
                {
                    if (0 == strncmp(&js[start], "true", strlen("true")))
                    {
                        /* subtract 1 so we don't end up at character
                                  AFTER the final char in token */
                        parser->pos += strlen("true") - 1;
                        break;
                    }
                    else if (0 == strncmp(&js[start], "false", strlen("false")))
                    {
                        /* subtract 1 so we don't end up at character
                                  AFTER the final char in token */
                        parser->pos += strlen("false") - 1;
                        break;
                    }
                    else if (0 == strncmp(&js[start], "null", strlen("null")))
                    {
                        /* subtract 1 so we don't end up at character
                                  AFTER the final char in token */
                        parser->pos += strlen("null") - 1;
                        break;
                    }
                    else
                    {
                        parser->pos = start;
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
                parser->pos = start;
                return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
            }
            break;
        }
    }
    return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
}


static JTOK_PARSE_STATUS_t
jtok_parse_string(jtok_parser_t *parser, jtoktok_t *tokens, size_t num_tokens)
{
    jtoktok_t *  token;
    int          start;
    char *       js  = parser->json;
    unsigned int len = parser->json_len;
    if (js[parser->pos] == '\"')
    {
        parser->pos++; /* advance to inside of quotes */
        start =
            parser->pos; /* token start is first character after the quote */
        for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
        {
            /* Quote: end of string */
            if (js[parser->pos] == '\"')
            {
                if (tokens == NULL)
                {
                    return JTOK_PARSE_STATUS_PARSE_OK;
                }
                token = jtok_alloc_token(parser, tokens, num_tokens);
                if (token == NULL)
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_NOMEM;
                }
                jtok_fill_token(token, JTOK_STRING, start, parser->pos);
                token->parent = parser->toksuper;
                return JTOK_PARSE_STATUS_PARSE_OK;
            }

            if (js[parser->pos] == '\\')
            {
                if (parser->pos + sizeof((char)'\"') < len)
                {
                    parser->pos++;
                    switch (js[parser->pos])
                    {
                        /* Allowed escaped symbols */
                        case '\"':
                        case '/':
                        case '\\':
                        case 'b':
                        case 'f':
                        case 'r':
                        case 'n':
                        case 't':
                        {
                        }
                        break;
                        case 'u': /* Allows escaped symbol \uXXXX */
                        {
                            parser->pos++; /* move to first escaped hex
                                              character */
                            unsigned int       i;
                            const unsigned int max_i = HEXCHAR_ESCAPE_SEQ_COUNT;
                            for (i = 0; i < max_i && parser->pos < len &&
                                        js[parser->pos] != '\0';
                                 i++)
                            {
                                if (!isxdigit(js[parser->pos]))
                                {
                                    /* reset parser position and return error */
                                    parser->pos = start;
                                    return JTOK_PARSE_STATUS_INVAL;
                                }
                                parser->pos++;
                            }
                            parser->pos--;
                        }
                        break;
                        default: /* Unexpected symbol */
                        {
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVAL;
                        }
                        break;
                    }
                }
            }
        }
        parser->pos = start;
        return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }
    else
    {
        /* parse_string was called on a non-string */
        return JTOK_PARSE_STATUS_UNKNOWN_ERROR;
    }
}


static JTOK_PARSE_STATUS_t
jtok_parse_array(jtok_parser_t *parser, jtoktok_t *tokens, size_t num_tokens)
{
    JTOK_PARSE_STATUS_t status = JTOK_PARSE_STATUS_PARSE_OK;
    unsigned int        start  = parser->pos;
    const char *        json   = parser->json;

    bool       element_type_found = false;
    jtoktype_t element_type;
    enum
    {
        ARRAY_START,
        ARRAY_VALUE,
        ARRAY_COMMA
    } expecting = ARRAY_START;

    if (tokens == NULL)
    {
        return status;
    }
    else if (json[parser->pos] != '[')
    {
        return JTOK_PARSE_STATUS_NON_ARRAY;
    }

    jtoktok_t *token = jtok_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        /*
         * Do not reset parser->pos because we want
         * caller to see which token maxed out the
         * pool
         */
        status = JTOK_PARSE_STATUS_NOMEM;
    }

    token->parent    = parser->toksuper;
    parser->toksuper = parser->toknext - 1;

    /* end of token will be populated when we find the closing brace */
    jtok_fill_token(token, JTOK_ARRAY, parser->pos, INVALID_ARRAY_INDEX);

    /* go inside the object */
    parser->pos++;

    for (; parser->pos < parser->json_len && json[parser->pos] != '\0' &&
           status == JTOK_PARSE_STATUS_PARSE_OK;
         parser->pos++)
    {
        switch (json[parser->pos])
        {
            case '{':
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_OBJECT)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_OBJECT;
                        }

                        int super = parser->toksuper;
                        status = jtok_parse_object(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }
                            expecting        = ARRAY_COMMA;
                            parser->toksuper = super;
                        }
                    }
                    break;
                    case ARRAY_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_ARRAY_SEPARATOR;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;

#ifdef SUBARRAYS
#error SUBARRAYS ARE CURRENTLY NOT IMPLEMENTED
            case '[':
            {
            }
            break;
#endif
            case ']':
            {
                if (expecting != ARRAY_COMMA)
                {
                    status = JTOK_PARSE_STATUS_ARRAY_SEPARATOR;
                }
                return status;
            }
            break;
            case '\"':
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_OBJECT)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_OBJECT;
                        }

                        int super = parser->toksuper;
                        status = jtok_parse_string(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }

                            expecting = ARRAY_COMMA;
                        }
                    }
                    break;
                    case ARRAY_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_ARRAY_SEPARATOR;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                continue; /* skip whitespce */
            case ',':
            {
                switch (expecting)
                {
                    case ARRAY_COMMA:
                    {
                        expecting = ARRAY_VALUE;
                    }
                    break;
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        status = JTOK_PARSE_STATUS_STRAY_COMMA;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '+':
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 't':
            case 'f':
            case 'n':
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_PRIMITIVE)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_PRIMITIVE;
                        }

                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            int super = parser->toksuper;
                            status    = jtok_parse_primitive(parser, tokens,
                                                          num_tokens);
                            if (status == JTOK_PARSE_STATUS_PARSE_OK)
                            {
                                if (super != NO_PARENT_IDX)
                                {
                                    tokens[super].size++;
                                }

                                expecting = ARRAY_COMMA;
                            }
                            parser->toksuper = super;
                        }
                    }
                    break;
                    case ARRAY_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_STRAY_COMMA;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (status == JTOK_PARSE_STATUS_PARSE_OK)
    {
        parser->pos = start;
        status      = JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }

    return status;
}


static JTOK_PARSE_STATUS_t
jtok_parse_object(jtok_parser_t *parser, jtoktok_t *tokens, size_t num_tokens)
{
    JTOK_PARSE_STATUS_t status = JTOK_PARSE_STATUS_PARSE_OK;

    unsigned int start = parser->pos;
    const char * json  = parser->json;
    unsigned int len   = parser->json_len;
    enum
    {
        OBJECT_KEY,
        OBJECT_COLON,
        OBJECT_VALUE,
        OBJECT_COMMA,
    } expecting = OBJECT_KEY;

    if (tokens == NULL)
    {
        return status;
    }
    else if (json[parser->pos] != '{')
    {
        return JTOK_PARSE_STATUS_NON_OBJECT;
    }

    jtoktok_t *token = jtok_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        /*
         * Do not reset parser->pos because we want
         * caller to see which token maxed out the
         * pool
         */
        status = JTOK_PARSE_STATUS_NOMEM;
    }

    /* If the object has a parent key, increase that key's size */
    token->parent          = parser->toksuper;
    parser->toksuper       = parser->toknext - 1;
    int object_token_index = parser->toksuper;

    /* end of token will be populated when we find the closing brace */
    jtok_fill_token(token, JTOK_OBJECT, parser->pos, INVALID_ARRAY_INDEX);

    /* go inside the object */
    parser->pos++;


    for (; parser->pos < len && json[parser->pos] != '\0' &&
           status == JTOK_PARSE_STATUS_PARSE_OK;
         parser->pos++)
    {
        switch (json[parser->pos])
        {
            case '{':
            {
                switch (expecting)
                {
                    case OBJECT_KEY:
                    {
                        status = JTOK_PARSE_STATUS_OBJ_NOKEY;
                    }
                    break;
                    case OBJECT_COLON:
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COLON;
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        int super = parser->toksuper;
                        status = jtok_parse_object(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }
                            parser->toksuper = super;
                            expecting        = OBJECT_COMMA;
                        }
                    }
                    break;
                    case OBJECT_COMMA:
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '[':
            {
                switch (expecting)
                {
                    case OBJECT_KEY:
                    {
                        status = JTOK_PARSE_STATUS_OBJ_NOKEY;
                    }
                    break;
                    case OBJECT_COLON:
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COLON;
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        int super = parser->toksuper;
                        status = jtok_parse_array(parser, tokens, num_tokens);

                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }
                            else
                            {
                                /* Keys must have a parent token */
                                status = JTOK_PARSE_STATUS_INVALID_PARENT;
                            }

                            expecting = OBJECT_COMMA;
                        }
                    }
                    break;
                    case OBJECT_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '}':
            {
                switch (expecting)
                {
                    /* Technically we should be checking if this is the very
                     * first token in the object, because
                     * {"key1" : "value1", "key2" : "value2",} is invalid
                     * from the trailing comma
                     * (transition to expecting==key only occurs when finding
                     * comma or when we START parsing the object)
                     *
                     * So in cases where we have transitioned from a comma,
                     * if we find '}' then it means we have a trailing
                     * comma inside the object
                     */
                    case OBJECT_KEY:
                    {
                        jtoktok_t *parent = &tokens[object_token_index];
                        if (parent->type != JTOK_OBJECT || parser->toknext == 0)
                        {
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        else
                        {
                            parent->end      = parser->pos + 1;
                            parser->toksuper = parent->parent;
                        }
                        return status;
                    }
                    break;

                    /* instead of a comma, we can find end of object '}' */
                    case OBJECT_COMMA:
                    {
                        jtoktok_t *parent = &tokens[object_token_index];
                        if (parent->type != JTOK_OBJECT || parser->toknext == 0)
                        {
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        else
                        {
                            parent->end      = parser->pos + 1;
                            parser->toksuper = parent->parent;
                        }
                        return status;
                    }
                    break;
                    case OBJECT_COLON:
                    {
                        status = JTOK_PARSE_STATUS_KEY_NO_VAL;
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        status = JTOK_PARSE_STATUS_KEY_NO_VAL;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '\"':
            {
                jtoktok_t *parent = &tokens[parser->toksuper];
                switch (expecting)
                {
                    case OBJECT_KEY:
                    {
                        if (parent->type == JTOK_OBJECT)
                        {
                            int super = parser->toksuper;
                            status =
                                jtok_parse_string(parser, tokens, num_tokens);
                            if (status == JTOK_PARSE_STATUS_PARSE_OK)
                            {
                                tokens[super].size++;
                            }
                            expecting = OBJECT_COLON;
                        }
                        else
                        {
                            status = JTOK_PARSE_STATUS_INVALID_PARENT;
                        }
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        if (parent->type == JTOK_STRING)
                        {
                            if (parent->size != 0)
                            {
                                /* an object key can only have 1 value */
                                status = JTOK_PARSE_STATUS_KEY_MULTIPLE_VAL;
                            }
                            else
                            {
                                int super = parser->toksuper;
                                status    = jtok_parse_string(parser, tokens,
                                                           num_tokens);
                                if (status == JTOK_PARSE_STATUS_PARSE_OK)
                                {
                                    tokens[super].size++;
                                }
                                expecting = OBJECT_COMMA;
                            }
                        }
                        else
                        {
                            status = JTOK_PARSE_STATUS_INVALID_PARENT;
                        }
                    }
                    break;
                    case OBJECT_COLON: /* found " when expecting ':' */
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COLON;
                    }
                    break;
                    case OBJECT_COMMA: /* found " when expecting ',' */
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COMMA;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                continue; /* skip whitespce */
            case ':':
            {
                if (expecting == OBJECT_COLON)
                {
                    expecting        = OBJECT_VALUE;
                    parser->toksuper = parser->toknext - 1;
                }
                else
                {
                    parser->pos = start;
                    status      = JTOK_PARSE_STATUS_INVAL;
                }
            }
            break;
            case ',':
            {
                if (expecting == OBJECT_COMMA)
                {
                    expecting         = OBJECT_KEY;
                    jtoktok_t *parent = &tokens[parser->toksuper];
                    switch (parent->type)
                    {
                        case JTOK_PRIMITIVE:
                        case JTOK_STRING:
                        {
                            /*
                             * Update the toksuper index to
                             * the KEY's parent (the current object)
                             */
                            parser->toksuper = tokens[parser->toksuper].parent;
                        }
                        break;
                    }
                }
                else
                {
                    status = JTOK_PARSE_STATUS_OBJ_NOKEY;
                }
            }
            break;
            case '+':
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 't':
            case 'f':
            case 'n':
            {
                /* We must be expecting a value */
                if (expecting == OBJECT_VALUE)
                {
                    /* We're at the start of a primitive so validate parent type
                     */
                    jtoktok_t *parent = &tokens[parser->toksuper];
                    switch (parent->type)
                    {
                        case JTOK_OBJECT:
                        {
                            /* primitives cannot be keys (they are not
                             * quoted)
                             */
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        case JTOK_STRING:
                            if (parent->size != 0)
                            {
                                /* an object key can only have 1 value */
                                parser->pos = start;
                                status      = JTOK_PARSE_STATUS_INVAL;
                            }
                            break;
                        default:
                        {
                            /*
                             * If we're inside parse_object,
                             * other types cannot be parent tokens
                             */
                            status = JTOK_PARSE_STATUS_INVAL;
                        }
                        break;
                    }

                    if (status == JTOK_PARSE_STATUS_PARSE_OK)
                    {
                        status =
                            jtok_parse_primitive(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (parser->toksuper != NO_PARENT_IDX)
                            {
                                tokens[parser->toksuper].size++;
                            }
                            expecting = OBJECT_COMMA;
                        }
                    }
                }
                else
                {
                    /* move pos to start of the key that's missing the value */
                    parser->pos = tokens[parser->toksuper].start;
                    status      = JTOK_PARSE_STATUS_KEY_NO_VAL;
                }
            }
            break;
            default: /* unexpected character */
            {
                parser->pos = start;
                status      = JTOK_PARSE_STATUS_INVAL;
            }
            break;
        } /* end of character switch statement */
    }

    if (status == JTOK_PARSE_STATUS_PARSE_OK)
    {
        /* If we didnt find the } to close current object, we have partial JSON
         */
        parser->pos = start;
        status      = JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }

    return status;
}


static jtoktok_t *jtok_alloc_token(jtok_parser_t *parser, jtoktok_t *tokens,
                                   size_t num_tokens)
{
    jtoktok_t *tok;
    if (parser->toknext >= num_tokens)
    {
        return NULL;
    }
    tok        = &tokens[parser->toknext++];
    tok->start = tok->end = INVALID_ARRAY_INDEX;
    tok->size             = 0;
    tok->parent           = NO_PARENT_IDX;

#if defined(JTOK_STANDALONE_TOKENS)
    tok->json = parser->json;
#endif /*#if defined(JTOK_STANDALONE_TOKENS) */
    return tok;
}


static JTOK_PARSE_STATUS_t tokenChainIsValid(const jtok_parser_t *parser,
                                             jtoktok_t *          tokens)
{
    int i;
    for (i = parser->toknext - 1; i >= 0; i--)
    {
        /* Unmatched opened object or array */
        if (tokens[i].start != JTOK_STRING_INDEX_NONE)
        {
            if (tokens[i].end == JTOK_STRING_INDEX_NONE)
            {
                return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
            }
            else
            {
                if (i == 0)
                {
                    if (tokens[i].parent != NO_PARENT_IDX)
                    {
                        return JTOK_PARSE_STATUS_UNKNOWN_ERROR;
                    }

                    if (tokens[i].type != JTOK_OBJECT)
                    {
                        return JTOK_PARSE_STATUS_NON_OBJECT;
                    }
                }
                else
                {
                    switch (tokens[i].type)
                    {
                        case JTOK_OBJECT:
                        {
                            int parent_idx = tokens[i].parent;
                            switch (tokens[parent_idx].type)
                            {
                                /* normally, objects' parent type must be string
                                 * (key) */
                                case JTOK_STRING:
                                /* but also, objects can be elements of an array
                                 */
                                /* ex: { "key" : [ {}, {}, {}]} is valid */
                                case JTOK_ARRAY:
                                    break;
                                default:
                                    return JTOK_PARSE_STATUS_OBJECT_INVALID_PARENT;
                            }
                        }
                        break;
                        case JTOK_ARRAY:
                        {
                            if (i == 0)
                            {
                                /* first token cant be array */
                                return JTOK_PARSE_STATUS_INVAL;
                            }
                            else
                            {
                                int parent_idx = tokens[i].parent;
                                if (tokens[parent_idx].type != JTOK_STRING)
                                {
                                    return JTOK_PARSE_STATUS_OBJECT_INVALID_PARENT;
                                }
                            }
                        }
                        break;
                        case JTOK_STRING:
                        {
                            int parent_idx = tokens[i].parent;

                            /* keys require values */
                            if (tokens[parent_idx].type == JTOK_OBJECT)
                            {
                                if (tokens[i].size == 0)
                                {
                                    return JTOK_PARSE_STATUS_KEY_NO_VAL;
                                }
                            }
                        }
                        break;
                        case JTOK_PRIMITIVE:
                        {
                            if (tokens[i].size == 0)
                            {
                                int parent_idx = tokens[i].parent;
                                if (parent_idx == NO_PARENT_IDX)
                                {
                                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                                }
                                else
                                {
                                    switch (tokens[parent_idx].type)
                                    {
                                        /* normally, objects' parent type must
                                         * be string (key) */
                                        case JTOK_STRING:
                                        /* but also, objects can be elements of
                                         * an array */
                                        /* ex: { "key" : [ {}, {}, {}]} is valid
                                         */
                                        case JTOK_ARRAY:
                                            break;
                                        default:
                                            return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                                    }
                                }
                            }
                            else
                            {
                                return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    return JTOK_PARSE_STATUS_PARSE_OK;
}


jtok_parser_t jtok_new_parser(const char *nul_terminated_json)
{
    jtok_parser_t parser;
    parser.pos      = 0;
    parser.toknext  = 0;
    parser.toksuper = NO_PARENT_IDX;
    parser.json     = (char *)nul_terminated_json;
    parser.json_len = strlen(nul_terminated_json);
    return parser;
}


bool jtok_tokenIsKey(jtoktok_t token)
{
    if (token.type == JTOK_STRING)
    {
        if (token.size == 1)
        {
            return true;
        }
    }
    return false;
}


int jtok_token_tostr(char *buf, unsigned int size, const char *json,
                     jtoktok_t token)
{
    if (buf != NULL)
    {
        int blen = 0;
        blen += snprintf(buf + blen, size - blen, "token : ");
        for (unsigned int k = token.start; k < token.end; k++)
        {
            blen += snprintf(buf + blen, size - blen, "%c", json[k]);
        }
        blen += snprintf(buf + blen, size - blen, "\n");
        blen += snprintf(buf + blen, size - blen, "type: %s\n",
                         jtok_toktypename(token.type));

#ifdef DEBUG
        blen += snprintf(buf + blen, size - blen, "start : %u\n", token.start);
        blen += snprintf(buf + blen, size - blen, "end : %u\n", token.end);
#endif /* ifdef DEBUG */

        return blen;
    }
    else
    {
        return -1;
    }
}


jtok_value_map_t parse_value(const jtoktok_t *token)
{


    /** @todo */
}
