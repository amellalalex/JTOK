
/* JTOK.C
   State-machine based JSON parser for embedded systems inspired by JSMN.
   Author: Carl Mattatall
*/

#include "jtok.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

static parseRetval_t retokenize(jparser_t *parser, const char *json, uword_t num_tokens);

static void fill_token(jtok_t *token,
                       jtokType_t type,
                       word_t start,
                       word_t end);

static jtok_t *alloc_token(jparser_t *parser,
                           uword_t num_tokens);

#include <stdio.h.>
#include <assert.h>

static void tprint(const char *json, jtok_t t)
{
  int i;
  for (i = t.start; i < t.end; i++)
  {
    printf("%c", json[i]);
  }
  printf("\ntype   : %d"
         "\nstart  : %d"
         "\nend    : %d"
         "\nsize   : %d"
         "\nparent : %d\n\n",
         t.type,
         t.start,
         t.end,
         t.size,
         t.parent);
}

parseRetval_t jtokenize(jparser_t *parser, const char *json)
{
  parseRetval_t result;

  // need to make this null so it gets assigned an address on heap
  parser->tokens = NULL;

  if (json == NULL)
  {
    result.status = JPARSE_NULLSTR;
  }
  else
  {
    uword_t num_tokens = BASE_TKN_GUESS;
    do
    {
      num_tokens = num_tokens * 2 + 1;
      parser->tokens = (jtok_t *)realloc(parser->tokens, sizeof(jtok_t) * num_tokens);
      result = retokenize(parser, json, num_tokens);
    } while (result.status == JPARSE_NOHEAP);

    if (result.status != JPARSE_OK)
    {
      free(parser->tokens);
      parser->tokens = NULL;
    }
  }
  return result;
}

static parseRetval_t retokenize(jparser_t *parser,
                                const char *json,
                                uword_t num_tokens)
{
  parseRetval_t result;
  result.status = JPARSE_OK;
  result.cnt = 0;

  // the parse state
  enum
  {
    OBJECT,
    STRING,
    PRIMTIVE,
  } state;
  state = OBJECT;

  //temporary variables
  jtok_t *token;

  //initialize the parser
  parser->pos = 0;
  parser->start = 0;
  parser->toknext = 0;
  parser->toksuper = -1;

  if (json == NULL)
  {
    result.status = JPARSE_NULLSTR;
  }
  else
  {
    for (; result.status == JPARSE_OK && json[parser->pos] != '\0'; parser->pos += 1)
    {
      jtokType_t type;

      if (iscntrl(json[parser->pos]) == 0) //if current char is not a control character
      {
        printf("%d : >%c<  state = %d,  cnt = %d, super = %d, status = %d | ", parser->pos, json[parser->pos], state, result.cnt, parser->toksuper, result.status);
        switch (state)
        {
        case OBJECT:
          switch (json[parser->pos])
          {
          case '{': //start of parent object
          case '[':
            printf("found start of obj | ");
            result.cnt += 1;
            if ((token = alloc_token(parser, num_tokens)) == NULL)
            {
              result.status = JPARSE_NOHEAP;
            }
            else
            {
              //if object has a parent node, increment the parent node's size
              if (parser->toksuper != -1)
              {
                parser->tokens[parser->toksuper].size += 1;
                token->parent = parser->toksuper;
              }
              token->type = (json[parser->pos] == '{' ? JTOK_OBJ : JTOK_ARR);
              token->start = parser->pos;
              parser->toksuper = parser->toknext - 1;
            }
            break;
          case '}': //end of parent object
          case ']':
            printf("found end of object | ");
            type = (json[parser->pos] == '}' ? JTOK_OBJ : JTOK_ARR);
            if (parser->toknext < 1)
            {
              result.status = JPARSE_INVAL;
            }
            else
            {
              token = &(parser->tokens[parser->toknext - 1]);

              //todo: this loop can be cleaned up
              for (;;)
              {

                if (token->start != -1) //if we've found the start of the token,
                {
                  if (token->end == -1) //but not the end
                  {
                    if (token->type == type) //and the bracket types of start and end match
                    {
                      token->end = parser->pos + 1;
                      parser->toksuper = token->parent;
                    }
                    else
                    {
                      result.status = JPARSE_INVAL;
                    }
                    break;
                  }
                }

                if (token->parent == -1)
                {
                  break;
                }
                token = &(parser->tokens[token->parent]);
              }
            }
            break;
          case '\"':
            printf("found start quote! | ");
            parser->start = parser->pos;
            state = STRING;
            break;
          case '\t': //skip whitespace when we're in object parsing mode
          case '\r':
          case '\n':
          case ' ':
            break;
          case ':':

            printf("found >:<. parser->toksuper == %d, newvalue of parser->toksuper == %d | ", parser->toksuper, parser->toknext - 1);
            //store the index of current object 

            //the token after the >:< is owned by the token before the >:<
            parser->toksuper = parser->toknext - 1;
            break;
          case ',':
            printf(" found comma! |");
            if (
                parser->tokens[parser->toksuper].type != JTOK_ARR &&
                parser->tokens[parser->toksuper].type != JTOK_OBJ)
            {
              parser->toksuper = parser->tokens[parser->toksuper].parent;
            }
            else //NUM OR STRING MUST COME BEFORE COMMAS
            {
              result.status = JPARSE_INVAL;
              break;
            }
            break;
          case '-': /* primitives are: numbers and booleans */
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
            printf(" found a primitive type! | ");
            /* And they must not be keys of a parent object (keys MUST be quoted)*/

            printf("superior node type == %d | ", (&parser->tokens[parser->toksuper])->type);
            switch ((&parser->tokens[parser->toksuper])->type)
            {
            case JTOK_OBJ:
              result.status = JPARSE_INVAL;
              break;
            case JTOK_ARR:
              if ((&parser->tokens[parser->toksuper])->size != 0)
              {
                result.status = JPARSE_INVAL;
              }
              break;
            case JTOK_NUM:
            case JTOK_STR:
              printf("superior node is string | ");
              parser->start = parser->pos;
              state = PRIMTIVE;
              break;
            default:
              //critical
              break;
            }
            break;
          default: //unexpected character
            result.status = JPARSE_INVAL;
            break;
          }
          break;
        case STRING:

          switch (json[parser->pos])
          {
          case '\"':
            printf("found end quote! | ");

            if ((token = alloc_token(parser, num_tokens)) == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOHEAP;
            }
            else
            {
              // start + 1 to skip beginning quote
              fill_token(token, JTOK_STR, parser->start + 1, parser->pos);
              token->parent = parser->toksuper; //link to parent node
              printf("assigned %d to tkn->parent | ", parser->toksuper);

              result.cnt += 1;
              printf("increasing count! now cnt = %d |", result.cnt);

              parser->tokens[token->parent].size += 1; //increase the size of the parent token

              printf("tkn has a parent. increasing parent size. now parent size = %d |", parser->tokens[parser->toksuper].size);
              state = OBJECT;
            }
            break;
          //invalid characters. CURRENTLY WE DO NOT SUPPORT STRING : STRING, ONLY STRING : NUM.
          //TODO: ADD SUPPORT FOR JSONS WITH STRING : NUM key-val formatting.
          case ':':
          case ',':
          case '}':
          case ']':
          case '[':
          case '{':
          case '\'':
          case '-':
            result.status = JPARSE_INVAL;
            break;
          }
          break;
        case PRIMTIVE:
          switch (json[parser->pos])
          {
          //characters that would delimit a primitive token
          case '\t':
          case '\r':
          case '\n':
          case ' ':
          case ',':
          case ']':
          case '}':

            printf("found end of primitive | ");
            if ((token = alloc_token(parser, num_tokens)) == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOHEAP;
            }
            else
            {
              fill_token(token, JTOK_NUM, parser->start, parser->pos);
              token->parent = parser->toksuper; //link to parent

              //parser->pos--; //NOT SURE WHY I NEED THIS HERE

              result.cnt += 1;
              parser->tokens[token->parent].size += 1; //increase size of parent object
              state = OBJECT;
            }
            break;
          case ':': //invalid characters
          case '{':
          case '[':
          case '\'':
          case '\"':
            result.status = JPARSE_INVAL;
            break;
          }
          break;
        default:
          //critical: somehow a pointer corrupted parsing state
          break;
        }
      }
      else //control characters are invalid
      {
        result.status = JPARSE_INVAL;
        break;
      }

      printf("\n");
    }

    word_t i;
    for (i = 0; i < result.cnt; i++)
    { 
      printf("tkn[%d] : ", i);
      tprint(json, parser->tokens[i]);
    }

    word_t idx;
    for (idx = parser->toknext - 1; idx >= 0; idx--)
    {
      // Unmatched opened object or array
      if (parser->tokens[idx].start != -1)
      {
        if (parser->tokens[idx].end == -1)
        { 
          printf("FOUND AN INCOMPLETE TOKEN\n");
          result.status = JPARSE_PARTIAL;
          break;
        }
      }
    }
  }
  return result;
}

static jtok_t *alloc_token(jparser_t *parser,
                           uword_t num_tokens)
{
  jtok_t *tok;
  if (parser->toknext >= num_tokens)
  {
    return NULL;
  }

  tok = &parser->tokens[parser->toknext];
  parser->toknext += 1;

  tok->start = tok->end = -1;
  tok->size = 0;
  tok->parent = -1;
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static void fill_token(jtok_t *token,
                       jtokType_t type,
                       word_t start,
                       word_t end)
{
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}