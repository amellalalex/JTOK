/**
 * @file empty_object_parse_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test parsing of empty objects
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 */

#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

/* These are the various permutations of
 * whitespace for an empty object */
static const char *valid_jsons[] = {
    "{}",
    "{} ",
    "{}\t",
    "{}\n",
    " {}",
    " {} ",
    " {}\t",
    " {}\n",
    "\t{}",
    "\t{} ",
    "\t{}\t",
    "\t{}\n",
    "\t {}",
    "\t {} ",
    "\t {}\t",
    "\t {}\n",
    "\n{}",
    "\n{} ",
    "\n{}\t",
    "\n{}\n",
    "\n {}",
    "\n {} ",
    "\n {}\t",
    "\n {}\n",
    "\n\t{}",
    "\n\t{} ",
    "\n\t{}\t",
    "\n\t{}\n",
    "\n\t {}",
    "\n\t {} ",
    "\n\t {}\t",
    "{}\n",
    "{} \n",
    "{}\t\n",
    "{}\n\n",
    " {}\n",
    " {} \n",
    " {}\t\n",
    " {}\n\n",
    "\t{}\n",
    "\t{} \n",
    "\t{}\t\n",
    "\t{}\n\n",
    "\t {}\n",
    "\t {} \n",
    "\t {}\t\n",
    "\t {}\n\n",
    "\n{}\n",
    "\n{} \n",
    "\n{}\t\n",
    "\n{}\n\n",
    "\n {}\n",
    "\n {} \n",
    "\n {}\t\n",
    "\n {}\n\n",
    "\n\t{}\n",
    "\n\t{} \n",
    "\n\t{}\t\n",
    "\n\t{}\n\n",
    "\n\t {}\n",
    "\n\t {} \n",
    "\n\t {}\t\n",
    "{ }",
    "{ } ",
    "{ }\t",
    "{ }\n",
    " { }",
    " { } ",
    " { }\t",
    " { }\n",
    "\t{ }",
    "\t{ } ",
    "\t{ }\t",
    "\t{ }\n",
    "\t { }",
    "\t { } ",
    "\t { }\t",
    "\t { }\n",
    "\n{ }",
    "\n{ } ",
    "\n{ }\t",
    "\n{ }\n",
    "\n { }",
    "\n { } ",
    "\n { }\t",
    "\n { }\n",
    "\n\t{ }",
    "\n\t{ } ",
    "\n\t{ }\t",
    "\n\t{ }\n",
    "\n\t { }",
    "\n\t { } ",
    "\n\t { }\t",
    "{ }\n",
    "{ } \n",
    "{ }\t\n",
    "{ }\n\n",
    " { }\n",
    " { } \n",
    " { }\t\n",
    " { }\n\n",
    "\t{ }\n",
    "\t{ } \n",
    "\t{ }\t\n",
    "\t{ }\n\n",
    "\t { }\n",
    "\t { } \n",
    "\t { }\t\n",
    "\t { }\n\n",
    "\n{ }\n",
    "\n{ } \n",
    "\n{ }\t\n",
    "\n{ }\n\n",
    "\n { }\n",
    "\n { } \n",
    "\n { }\t\n",
    "\n { }\n\n",
    "\n\t{ }\n",
    "\n\t{ } \n",
    "\n\t{ }\t\n",
    "\n\t{ }\n\n",
    "\n\t { }\n",
    "\n\t { } \n",
    "\n\t { }\t\n",
    "{ \t}",
    "{ \t} ",
    "{ \t}\t",
    "{ \t}\n",
    " { \t}",
    " { \t} ",
    " { \t}\t",
    " { \t}\n",
    "\t{ \t}",
    "\t{ \t} ",
    "\t{ \t}\t",
    "\t{ \t}\n",
    "\t { \t}",
    "\t { \t} ",
    "\t { \t}\t",
    "\t { \t}\n",
    "\n{ \t}",
    "\n{ \t} ",
    "\n{ \t}\t",
    "\n{ \t}\n",
    "\n { \t}",
    "\n { \t} ",
    "\n { \t}\t",
    "\n { \t}\n",
    "\n\t{ \t}",
    "\n\t{ \t} ",
    "\n\t{ \t}\t",
    "\n\t{ \t}\n",
    "\n\t { \t}",
    "\n\t { \t} ",
    "\n\t { \t}\t",
    "{ \t}\n",
    "{ \t} \n",
    "{ \t}\t\n",
    "{ \t}\n\n",
    " { \t}\n",
    " { \t} \n",
    " { \t}\t\n",
    " { \t}\n\n",
    "\t{ \t}\n",
    "\t{ \t} \n",
    "\t{ \t}\t\n",
    "\t{ \t}\n\n",
    "\t { \t}\n",
    "\t { \t} \n",
    "\t { \t}\t\n",
    "\t { \t}\n\n",
    "\n{ \t}\n",
    "\n{ \t} \n",
    "\n{ \t}\t\n",
    "\n{ \t}\n\n",
    "\n { \t}\n",
    "\n { \t} \n",
    "\n { \t}\t\n",
    "\n { \t}\n\n",
    "\n\t{ \t}\n",
    "\n\t{ \t} \n",
    "\n\t{ \t}\t\n",
    "\n\t{ \t}\n\n",
    "\n\t { \t}\n",
    "\n\t { \t} \n",
    "\n\t { \t}\t\n",
    "{\"key\":{}}",
    "{\"key\":{}} ",
    "{\"key\":{}}\t",
    "{\"key\":{}}\n",
    " {\"key\":{}}",
    " {\"key\":{}} ",
    " {\"key\":{}}\t",
    " {\"key\":{}}\n",
    "\t{\"key\":{}}",
    "\t{\"key\":{}} ",
    "\t{\"key\":{}}\t",
    "\t{\"key\":{}}\n",
    "\t {\"key\":{}}",
    "\t {\"key\":{}} ",
    "\t {\"key\":{}}\t",
    "\t {\"key\":{}}\n",
    "\n{\"key\":{}}",
    "\n{\"key\":{}} ",
    "\n{\"key\":{}}\t",
    "\n{\"key\":{}}\n",
    "\n {\"key\":{}}",
    "\n {\"key\":{}} ",
    "\n {\"key\":{}}\t",
    "\n {\"key\":{}}\n",
    "\n\t{\"key\":{}}",
    "\n\t{\"key\":{}} ",
    "\n\t{\"key\":{}}\t",
    "\n\t{\"key\":{}}\n",
    "\n\t {\"key\":{}}",
    "\n\t {\"key\":{}} ",
    "\n\t {\"key\":{}}\t",
    "{\"key\":{}}\n",
    "{\"key\":{}} \n",
    "{\"key\":{}}\t\n",
    "{\"key\":{}}\n\n",
    " {\"key\":{}}\n",
    " {\"key\":{}} \n",
    " {\"key\":{}}\t\n",
    " {\"key\":{}}\n\n",
    "\t{\"key\":{}}\n",
    "\t{\"key\":{}} \n",
    "\t{\"key\":{}}\t\n",
    "\t{\"key\":{}}\n\n",
    "\t {\"key\":{}}\n",
    "\t {\"key\":{}} \n",
    "\t {\"key\":{}}\t\n",
    "\t {\"key\":{}}\n\n",
    "\n{\"key\":{}}\n",
    "\n{\"key\":{}} \n",
    "\n{\"key\":{}}\t\n",
    "\n{\"key\":{}}\n\n",
    "\n {\"key\":{}}\n",
    "\n {\"key\":{}} \n",
    "\n {\"key\":{}}\t\n",
    "\n {\"key\":{}}\n\n",
    "\n\t{\"key\":{}}\n",
    "\n\t{\"key\":{}} \n",
    "\n\t{\"key\":{}}\t\n",
    "\n\t{\"key\":{}}\n\n",
    "\n\t {\"key\":{}}\n",
    "\n\t {\"key\":{}} \n",
    "\n\t {\"key\":{}}\t\n",
    "{\"key\":{} }",
    "{\"key\":{} } ",
    "{\"key\":{} }\t",
    "{\"key\":{} }\n",
    " {\"key\":{} }",
    " {\"key\":{} } ",
    " {\"key\":{} }\t",
    " {\"key\":{} }\n",
    "\t{\"key\":{} }",
    "\t{\"key\":{} } ",
    "\t{\"key\":{} }\t",
    "\t{\"key\":{} }\n",
    "\t {\"key\":{} }",
    "\t {\"key\":{} } ",
    "\t {\"key\":{} }\t",
    "\t {\"key\":{} }\n",
    "\n{\"key\":{} }",
    "\n{\"key\":{} } ",
    "\n{\"key\":{} }\t",
    "\n{\"key\":{} }\n",
    "\n {\"key\":{} }",
    "\n {\"key\":{} } ",
    "\n {\"key\":{} }\t",
    "\n {\"key\":{} }\n",
    "\n\t{\"key\":{} }",
    "\n\t{\"key\":{} } ",
    "\n\t{\"key\":{} }\t",
    "\n\t{\"key\":{} }\n",
    "\n\t {\"key\":{} }",
    "\n\t {\"key\":{} } ",
    "\n\t {\"key\":{} }\t",
    "{\"key\":{} }\n",
    "{\"key\":{} } \n",
    "{\"key\":{} }\t\n",
    "{\"key\":{} }\n\n",
    " {\"key\":{} }\n",
    " {\"key\":{} } \n",
    " {\"key\":{} }\t\n",
    " {\"key\":{} }\n\n",
    "\t{\"key\":{} }\n",
    "\t{\"key\":{} } \n",
    "\t{\"key\":{} }\t\n",
    "\t{\"key\":{} }\n\n",
    "\t {\"key\":{} }\n",
    "\t {\"key\":{} } \n",
    "\t {\"key\":{} }\t\n",
    "\t {\"key\":{} }\n\n",
    "\n{\"key\":{} }\n",
    "\n{\"key\":{} } \n",
    "\n{\"key\":{} }\t\n",
    "\n{\"key\":{} }\n\n",
    "\n {\"key\":{} }\n",
    "\n {\"key\":{} } \n",
    "\n {\"key\":{} }\t\n",
    "\n {\"key\":{} }\n\n",
    "\n\t{\"key\":{} }\n",
    "\n\t{\"key\":{} } \n",
    "\n\t{\"key\":{} }\t\n",
    "\n\t{\"key\":{} }\n\n",
    "\n\t {\"key\":{} }\n",
    "\n\t {\"key\":{} } \n",
    "\n\t {\"key\":{} }\t\n",
    "{\"key\":{} \t}",
    "{\"key\":{} \t} ",
    "{\"key\":{} \t}\t",
    "{\"key\":{} \t}\n",
    " {\"key\":{} \t}",
    " {\"key\":{} \t} ",
    " {\"key\":{} \t}\t",
    " {\"key\":{} \t}\n",
    "\t{\"key\":{} \t}",
    "\t{\"key\":{} \t} ",
    "\t{\"key\":{} \t}\t",
    "\t{\"key\":{} \t}\n",
    "\t {\"key\":{} \t}",
    "\t {\"key\":{} \t} ",
    "\t {\"key\":{} \t}\t",
    "\t {\"key\":{} \t}\n",
    "\n{\"key\":{} \t}",
    "\n{\"key\":{} \t} ",
    "\n{\"key\":{} \t}\t",
    "\n{\"key\":{} \t}\n",
    "\n {\"key\":{} \t}",
    "\n {\"key\":{} \t} ",
    "\n {\"key\":{} \t}\t",
    "\n {\"key\":{} \t}\n",
    "\n\t{\"key\":{} \t}",
    "\n\t{\"key\":{} \t} ",
    "\n\t{\"key\":{} \t}\t",
    "\n\t{\"key\":{} \t}\n",
    "\n\t {\"key\":{} \t}",
    "\n\t {\"key\":{} \t} ",
    "\n\t {\"key\":{} \t}\t",
    "{\"key\":{} \t}\n",
    "{\"key\":{} \t} \n",
    "{\"key\":{} \t}\t\n",
    "{\"key\":{} \t}\n\n",
    " {\"key\":{} \t}\n",
    " {\"key\":{} \t} \n",
    " {\"key\":{} \t}\t\n",
    " {\"key\":{} \t}\n\n",
    "\t{\"key\":{} \t}\n",
    "\t{\"key\":{} \t} \n",
    "\t{\"key\":{} \t}\t\n",
    "\t{\"key\":{} \t}\n\n",
    "\t {\"key\":{} \t}\n",
    "\t {\"key\":{} \t} \n",
    "\t {\"key\":{} \t}\t\n",
    "\t {\"key\":{} \t}\n\n",
    "\n{\"key\":{} \t}\n",
    "\n{\"key\":{} \t} \n",
    "\n{\"key\":{} \t}\t\n",
    "\n{\"key\":{} \t}\n\n",
    "\n {\"key\":{} \t}\n",
    "\n {\"key\":{} \t} \n",
    "\n {\"key\":{} \t}\t\n",
    "\n {\"key\":{} \t}\n\n",
    "\n\t{\"key\":{} \t}\n",
    "\n\t{\"key\":{} \t} \n",
    "\n\t{\"key\":{} \t}\t\n",
    "\n\t{\"key\":{} \t}\n\n",
    "\n\t {\"key\":{} \t}\n",
    "\n\t {\"key\":{} \t} \n",
    "\n\t {\"key\":{} \t}\t\n",
    "{\"key\":{ }}",
    "{\"key\":{ }} ",
    "{\"key\":{ }}\t",
    "{\"key\":{ }}\n",
    " {\"key\":{ }}",
    " {\"key\":{ }} ",
    " {\"key\":{ }}\t",
    " {\"key\":{ }}\n",
    "\t{\"key\":{ }}",
    "\t{\"key\":{ }} ",
    "\t{\"key\":{ }}\t",
    "\t{\"key\":{ }}\n",
    "\t {\"key\":{ }}",
    "\t {\"key\":{ }} ",
    "\t {\"key\":{ }}\t",
    "\t {\"key\":{ }}\n",
    "\n{\"key\":{ }}",
    "\n{\"key\":{ }} ",
    "\n{\"key\":{ }}\t",
    "\n{\"key\":{ }}\n",
    "\n {\"key\":{ }}",
    "\n {\"key\":{ }} ",
    "\n {\"key\":{ }}\t",
    "\n {\"key\":{ }}\n",
    "\n\t{\"key\":{ }}",
    "\n\t{\"key\":{ }} ",
    "\n\t{\"key\":{ }}\t",
    "\n\t{\"key\":{ }}\n",
    "\n\t {\"key\":{ }}",
    "\n\t {\"key\":{ }} ",
    "\n\t {\"key\":{ }}\t",
    "{\"key\":{ }}\n",
    "{\"key\":{ }} \n",
    "{\"key\":{ }}\t\n",
    "{\"key\":{ }}\n\n",
    " {\"key\":{ }}\n",
    " {\"key\":{ }} \n",
    " {\"key\":{ }}\t\n",
    " {\"key\":{ }}\n\n",
    "\t{\"key\":{ }}\n",
    "\t{\"key\":{ }} \n",
    "\t{\"key\":{ }}\t\n",
    "\t{\"key\":{ }}\n\n",
    "\t {\"key\":{ }}\n",
    "\t {\"key\":{ }} \n",
    "\t {\"key\":{ }}\t\n",
    "\t {\"key\":{ }}\n\n",
    "\n{\"key\":{ }}\n",
    "\n{\"key\":{ }} \n",
    "\n{\"key\":{ }}\t\n",
    "\n{\"key\":{ }}\n\n",
    "\n {\"key\":{ }}\n",
    "\n {\"key\":{ }} \n",
    "\n {\"key\":{ }}\t\n",
    "\n {\"key\":{ }}\n\n",
    "\n\t{\"key\":{ }}\n",
    "\n\t{\"key\":{ }} \n",
    "\n\t{\"key\":{ }}\t\n",
    "\n\t{\"key\":{ }}\n\n",
    "\n\t {\"key\":{ }}\n",
    "\n\t {\"key\":{ }} \n",
    "\n\t {\"key\":{ }}\t\n",
    "{\"key\":{ } }",
    "{\"key\":{ } } ",
    "{\"key\":{ } }\t",
    "{\"key\":{ } }\n",
    " {\"key\":{ } }",
    " {\"key\":{ } } ",
    " {\"key\":{ } }\t",
    " {\"key\":{ } }\n",
    "\t{\"key\":{ } }",
    "\t{\"key\":{ } } ",
    "\t{\"key\":{ } }\t",
    "\t{\"key\":{ } }\n",
    "\t {\"key\":{ } }",
    "\t {\"key\":{ } } ",
    "\t {\"key\":{ } }\t",
    "\t {\"key\":{ } }\n",
    "\n{\"key\":{ } }",
    "\n{\"key\":{ } } ",
    "\n{\"key\":{ } }\t",
    "\n{\"key\":{ } }\n",
    "\n {\"key\":{ } }",
    "\n {\"key\":{ } } ",
    "\n {\"key\":{ } }\t",
    "\n {\"key\":{ } }\n",
    "\n\t{\"key\":{ } }",
    "\n\t{\"key\":{ } } ",
    "\n\t{\"key\":{ } }\t",
    "\n\t{\"key\":{ } }\n",
    "\n\t {\"key\":{ } }",
    "\n\t {\"key\":{ } } ",
    "\n\t {\"key\":{ } }\t",
    "{\"key\":{ } }\n",
    "{\"key\":{ } } \n",
    "{\"key\":{ } }\t\n",
    "{\"key\":{ } }\n\n",
    " {\"key\":{ } }\n",
    " {\"key\":{ } } \n",
    " {\"key\":{ } }\t\n",
    " {\"key\":{ } }\n\n",
    "\t{\"key\":{ } }\n",
    "\t{\"key\":{ } } \n",
    "\t{\"key\":{ } }\t\n",
    "\t{\"key\":{ } }\n\n",
    "\t {\"key\":{ } }\n",
    "\t {\"key\":{ } } \n",
    "\t {\"key\":{ } }\t\n",
    "\t {\"key\":{ } }\n\n",
    "\n{\"key\":{ } }\n",
    "\n{\"key\":{ } } \n",
    "\n{\"key\":{ } }\t\n",
    "\n{\"key\":{ } }\n\n",
    "\n {\"key\":{ } }\n",
    "\n {\"key\":{ } } \n",
    "\n {\"key\":{ } }\t\n",
    "\n {\"key\":{ } }\n\n",
    "\n\t{\"key\":{ } }\n",
    "\n\t{\"key\":{ } } \n",
    "\n\t{\"key\":{ } }\t\n",
    "\n\t{\"key\":{ } }\n\n",
    "\n\t {\"key\":{ } }\n",
    "\n\t {\"key\":{ } } \n",
    "\n\t {\"key\":{ } }\t\n",
    "{\"key\":{ } \t}",
    "{\"key\":{ } \t} ",
    "{\"key\":{ } \t}\t",
    "{\"key\":{ } \t}\n",
    " {\"key\":{ } \t}",
    " {\"key\":{ } \t} ",
    " {\"key\":{ } \t}\t",
    " {\"key\":{ } \t}\n",
    "\t{\"key\":{ } \t}",
    "\t{\"key\":{ } \t} ",
    "\t{\"key\":{ } \t}\t",
    "\t{\"key\":{ } \t}\n",
    "\t {\"key\":{ } \t}",
    "\t {\"key\":{ } \t} ",
    "\t {\"key\":{ } \t}\t",
    "\t {\"key\":{ } \t}\n",
    "\n{\"key\":{ } \t}",
    "\n{\"key\":{ } \t} ",
    "\n{\"key\":{ } \t}\t",
    "\n{\"key\":{ } \t}\n",
    "\n {\"key\":{ } \t}",
    "\n {\"key\":{ } \t} ",
    "\n {\"key\":{ } \t}\t",
    "\n {\"key\":{ } \t}\n",
    "\n\t{\"key\":{ } \t}",
    "\n\t{\"key\":{ } \t} ",
    "\n\t{\"key\":{ } \t}\t",
    "\n\t{\"key\":{ } \t}\n",
    "\n\t {\"key\":{ } \t}",
    "\n\t {\"key\":{ } \t} ",
    "\n\t {\"key\":{ } \t}\t",
    "{\"key\":{ } \t}\n",
    "{\"key\":{ } \t} \n",
    "{\"key\":{ } \t}\t\n",
    "{\"key\":{ } \t}\n\n",
    " {\"key\":{ } \t}\n",
    " {\"key\":{ } \t} \n",
    " {\"key\":{ } \t}\t\n",
    " {\"key\":{ } \t}\n\n",
    "\t{\"key\":{ } \t}\n",
    "\t{\"key\":{ } \t} \n",
    "\t{\"key\":{ } \t}\t\n",
    "\t{\"key\":{ } \t}\n\n",
    "\t {\"key\":{ } \t}\n",
    "\t {\"key\":{ } \t} \n",
    "\t {\"key\":{ } \t}\t\n",
    "\t {\"key\":{ } \t}\n\n",
    "\n{\"key\":{ } \t}\n",
    "\n{\"key\":{ } \t} \n",
    "\n{\"key\":{ } \t}\t\n",
    "\n{\"key\":{ } \t}\n\n",
    "\n {\"key\":{ } \t}\n",
    "\n {\"key\":{ } \t} \n",
    "\n {\"key\":{ } \t}\t\n",
    "\n {\"key\":{ } \t}\n\n",
    "\n\t{\"key\":{ } \t}\n",
    "\n\t{\"key\":{ } \t} \n",
    "\n\t{\"key\":{ } \t}\t\n",
    "\n\t{\"key\":{ } \t}\n\n",
    "\n\t {\"key\":{ } \t}\n",
    "\n\t {\"key\":{ } \t} \n",
    "\n\t {\"key\":{ } \t}\t\n",
    "{\"key\":{\t}}",
    "{\"key\":{\t}} ",
    "{\"key\":{\t}}\t",
    "{\"key\":{\t}}\n",
    " {\"key\":{\t}}",
    " {\"key\":{\t}} ",
    " {\"key\":{\t}}\t",
    " {\"key\":{\t}}\n",
    "\t{\"key\":{\t}}",
    "\t{\"key\":{\t}} ",
    "\t{\"key\":{\t}}\t",
    "\t{\"key\":{\t}}\n",
    "\t {\"key\":{\t}}",
    "\t {\"key\":{\t}} ",
    "\t {\"key\":{\t}}\t",
    "\t {\"key\":{\t}}\n",
    "\n{\"key\":{\t}}",
    "\n{\"key\":{\t}} ",
    "\n{\"key\":{\t}}\t",
    "\n{\"key\":{\t}}\n",
    "\n {\"key\":{\t}}",
    "\n {\"key\":{\t}} ",
    "\n {\"key\":{\t}}\t",
    "\n {\"key\":{\t}}\n",
    "\n\t{\"key\":{\t}}",
    "\n\t{\"key\":{\t}} ",
    "\n\t{\"key\":{\t}}\t",
    "\n\t{\"key\":{\t}}\n",
    "\n\t {\"key\":{\t}}",
    "\n\t {\"key\":{\t}} ",
    "\n\t {\"key\":{\t}}\t",
    "{\"key\":{\t}}\n",
    "{\"key\":{\t}} \n",
    "{\"key\":{\t}}\t\n",
    "{\"key\":{\t}}\n\n",
    " {\"key\":{\t}}\n",
    " {\"key\":{\t}} \n",
    " {\"key\":{\t}}\t\n",
    " {\"key\":{\t}}\n\n",
    "\t{\"key\":{\t}}\n",
    "\t{\"key\":{\t}} \n",
    "\t{\"key\":{\t}}\t\n",
    "\t{\"key\":{\t}}\n\n",
    "\t {\"key\":{\t}}\n",
    "\t {\"key\":{\t}} \n",
    "\t {\"key\":{\t}}\t\n",
    "\t {\"key\":{\t}}\n\n",
    "\n{\"key\":{\t}}\n",
    "\n{\"key\":{\t}} \n",
    "\n{\"key\":{\t}}\t\n",
    "\n{\"key\":{\t}}\n\n",
    "\n {\"key\":{\t}}\n",
    "\n {\"key\":{\t}} \n",
    "\n {\"key\":{\t}}\t\n",
    "\n {\"key\":{\t}}\n\n",
    "\n\t{\"key\":{\t}}\n",
    "\n\t{\"key\":{\t}} \n",
    "\n\t{\"key\":{\t}}\t\n",
    "\n\t{\"key\":{\t}}\n\n",
    "\n\t {\"key\":{\t}}\n",
    "\n\t {\"key\":{\t}} \n",
    "\n\t {\"key\":{\t}}\t\n",
    "{\"key\":{\t} }",
    "{\"key\":{\t} } ",
    "{\"key\":{\t} }\t",
    "{\"key\":{\t} }\n",
    " {\"key\":{\t} }",
    " {\"key\":{\t} } ",
    " {\"key\":{\t} }\t",
    " {\"key\":{\t} }\n",
    "\t{\"key\":{\t} }",
    "\t{\"key\":{\t} } ",
    "\t{\"key\":{\t} }\t",
    "\t{\"key\":{\t} }\n",
    "\t {\"key\":{\t} }",
    "\t {\"key\":{\t} } ",
    "\t {\"key\":{\t} }\t",
    "\t {\"key\":{\t} }\n",
    "\n{\"key\":{\t} }",
    "\n{\"key\":{\t} } ",
    "\n{\"key\":{\t} }\t",
    "\n{\"key\":{\t} }\n",
    "\n {\"key\":{\t} }",
    "\n {\"key\":{\t} } ",
    "\n {\"key\":{\t} }\t",
    "\n {\"key\":{\t} }\n",
    "\n\t{\"key\":{\t} }",
    "\n\t{\"key\":{\t} } ",
    "\n\t{\"key\":{\t} }\t",
    "\n\t{\"key\":{\t} }\n",
    "\n\t {\"key\":{\t} }",
    "\n\t {\"key\":{\t} } ",
    "\n\t {\"key\":{\t} }\t",
    "{\"key\":{\t} }\n",
    "{\"key\":{\t} } \n",
    "{\"key\":{\t} }\t\n",
    "{\"key\":{\t} }\n\n",
    " {\"key\":{\t} }\n",
    " {\"key\":{\t} } \n",
    " {\"key\":{\t} }\t\n",
    " {\"key\":{\t} }\n\n",
    "\t{\"key\":{\t} }\n",
    "\t{\"key\":{\t} } \n",
    "\t{\"key\":{\t} }\t\n",
    "\t{\"key\":{\t} }\n\n",
    "\t {\"key\":{\t} }\n",
    "\t {\"key\":{\t} } \n",
    "\t {\"key\":{\t} }\t\n",
    "\t {\"key\":{\t} }\n\n",
    "\n{\"key\":{\t} }\n",
    "\n{\"key\":{\t} } \n",
    "\n{\"key\":{\t} }\t\n",
    "\n{\"key\":{\t} }\n\n",
    "\n {\"key\":{\t} }\n",
    "\n {\"key\":{\t} } \n",
    "\n {\"key\":{\t} }\t\n",
    "\n {\"key\":{\t} }\n\n",
    "\n\t{\"key\":{\t} }\n",
    "\n\t{\"key\":{\t} } \n",
    "\n\t{\"key\":{\t} }\t\n",
    "\n\t{\"key\":{\t} }\n\n",
    "\n\t {\"key\":{\t} }\n",
    "\n\t {\"key\":{\t} } \n",
    "\n\t {\"key\":{\t} }\t\n",
    "{\"key\":{\t} \t}",
    "{\"key\":{\t} \t} ",
    "{\"key\":{\t} \t}\t",
    "{\"key\":{\t} \t}\n",
    " {\"key\":{\t} \t}",
    " {\"key\":{\t} \t} ",
    " {\"key\":{\t} \t}\t",
    " {\"key\":{\t} \t}\n",
    "\t{\"key\":{\t} \t}",
    "\t{\"key\":{\t} \t} ",
    "\t{\"key\":{\t} \t}\t",
    "\t{\"key\":{\t} \t}\n",
    "\t {\"key\":{\t} \t}",
    "\t {\"key\":{\t} \t} ",
    "\t {\"key\":{\t} \t}\t",
    "\t {\"key\":{\t} \t}\n",
    "\n{\"key\":{\t} \t}",
    "\n{\"key\":{\t} \t} ",
    "\n{\"key\":{\t} \t}\t",
    "\n{\"key\":{\t} \t}\n",
    "\n {\"key\":{\t} \t}",
    "\n {\"key\":{\t} \t} ",
    "\n {\"key\":{\t} \t}\t",
    "\n {\"key\":{\t} \t}\n",
    "\n\t{\"key\":{\t} \t}",
    "\n\t{\"key\":{\t} \t} ",
    "\n\t{\"key\":{\t} \t}\t",
    "\n\t{\"key\":{\t} \t}\n",
    "\n\t {\"key\":{\t} \t}",
    "\n\t {\"key\":{\t} \t} ",
    "\n\t {\"key\":{\t} \t}\t",
    "{\"key\":{\t} \t}\n",
    "{\"key\":{\t} \t} \n",
    "{\"key\":{\t} \t}\t\n",
    "{\"key\":{\t} \t}\n\n",
    " {\"key\":{\t} \t}\n",
    " {\"key\":{\t} \t} \n",
    " {\"key\":{\t} \t}\t\n",
    " {\"key\":{\t} \t}\n\n",
    "\t{\"key\":{\t} \t}\n",
    "\t{\"key\":{\t} \t} \n",
    "\t{\"key\":{\t} \t}\t\n",
    "\t{\"key\":{\t} \t}\n\n",
    "\t {\"key\":{\t} \t}\n",
    "\t {\"key\":{\t} \t} \n",
    "\t {\"key\":{\t} \t}\t\n",
    "\t {\"key\":{\t} \t}\n\n",
    "\n{\"key\":{\t} \t}\n",
    "\n{\"key\":{\t} \t} \n",
    "\n{\"key\":{\t} \t}\t\n",
    "\n{\"key\":{\t} \t}\n\n",
    "\n {\"key\":{\t} \t}\n",
    "\n {\"key\":{\t} \t} \n",
    "\n {\"key\":{\t} \t}\t\n",
    "\n {\"key\":{\t} \t}\n\n",
    "\n\t{\"key\":{\t} \t}\n",
    "\n\t{\"key\":{\t} \t} \n",
    "\n\t{\"key\":{\t} \t}\t\n",
    "\n\t{\"key\":{\t} \t}\n\n",
    "\n\t {\"key\":{\t} \t}\n",
    "\n\t {\"key\":{\t} \t} \n",
    "\n\t {\"key\":{\t} \t}\t\n",
    "{\"key\":{\n}}",
    "{\"key\":{\n}} ",
    "{\"key\":{\n}}\t",
    "{\"key\":{\n}}\n",
    " {\"key\":{\n}}",
    " {\"key\":{\n}} ",
    " {\"key\":{\n}}\t",
    " {\"key\":{\n}}\n",
    "\t{\"key\":{\n}}",
    "\t{\"key\":{\n}} ",
    "\t{\"key\":{\n}}\t",
    "\t{\"key\":{\n}}\n",
    "\t {\"key\":{\n}}",
    "\t {\"key\":{\n}} ",
    "\t {\"key\":{\n}}\t",
    "\t {\"key\":{\n}}\n",
    "\n{\"key\":{\n}}",
    "\n{\"key\":{\n}} ",
    "\n{\"key\":{\n}}\t",
    "\n{\"key\":{\n}}\n",
    "\n {\"key\":{\n}}",
    "\n {\"key\":{\n}} ",
    "\n {\"key\":{\n}}\t",
    "\n {\"key\":{\n}}\n",
    "\n\t{\"key\":{\n}}",
    "\n\t{\"key\":{\n}} ",
    "\n\t{\"key\":{\n}}\t",
    "\n\t{\"key\":{\n}}\n",
    "\n\t {\"key\":{\n}}",
    "\n\t {\"key\":{\n}} ",
    "\n\t {\"key\":{\n}}\t",
    "{\"key\":{\n}}\n",
    "{\"key\":{\n}} \n",
    "{\"key\":{\n}}\t\n",
    "{\"key\":{\n}}\n\n",
    " {\"key\":{\n}}\n",
    " {\"key\":{\n}} \n",
    " {\"key\":{\n}}\t\n",
    " {\"key\":{\n}}\n\n",
    "\t{\"key\":{\n}}\n",
    "\t{\"key\":{\n}} \n",
    "\t{\"key\":{\n}}\t\n",
    "\t{\"key\":{\n}}\n\n",
    "\t {\"key\":{\n}}\n",
    "\t {\"key\":{\n}} \n",
    "\t {\"key\":{\n}}\t\n",
    "\t {\"key\":{\n}}\n\n",
    "\n{\"key\":{\n}}\n",
    "\n{\"key\":{\n}} \n",
    "\n{\"key\":{\n}}\t\n",
    "\n{\"key\":{\n}}\n\n",
    "\n {\"key\":{\n}}\n",
    "\n {\"key\":{\n}} \n",
    "\n {\"key\":{\n}}\t\n",
    "\n {\"key\":{\n}}\n\n",
    "\n\t{\"key\":{\n}}\n",
    "\n\t{\"key\":{\n}} \n",
    "\n\t{\"key\":{\n}}\t\n",
    "\n\t{\"key\":{\n}}\n\n",
    "\n\t {\"key\":{\n}}\n",
    "\n\t {\"key\":{\n}} \n",
    "\n\t {\"key\":{\n}}\t\n",
    "{\"key\":{\n} }",
    "{\"key\":{\n} } ",
    "{\"key\":{\n} }\t",
    "{\"key\":{\n} }\n",
    " {\"key\":{\n} }",
    " {\"key\":{\n} } ",
    " {\"key\":{\n} }\t",
    " {\"key\":{\n} }\n",
    "\t{\"key\":{\n} }",
    "\t{\"key\":{\n} } ",
    "\t{\"key\":{\n} }\t",
    "\t{\"key\":{\n} }\n",
    "\t {\"key\":{\n} }",
    "\t {\"key\":{\n} } ",
    "\t {\"key\":{\n} }\t",
    "\t {\"key\":{\n} }\n",
    "\n{\"key\":{\n} }",
    "\n{\"key\":{\n} } ",
    "\n{\"key\":{\n} }\t",
    "\n{\"key\":{\n} }\n",
    "\n {\"key\":{\n} }",
    "\n {\"key\":{\n} } ",
    "\n {\"key\":{\n} }\t",
    "\n {\"key\":{\n} }\n",
    "\n\t{\"key\":{\n} }",
    "\n\t{\"key\":{\n} } ",
    "\n\t{\"key\":{\n} }\t",
    "\n\t{\"key\":{\n} }\n",
    "\n\t {\"key\":{\n} }",
    "\n\t {\"key\":{\n} } ",
    "\n\t {\"key\":{\n} }\t",
    "{\"key\":{\n} }\n",
    "{\"key\":{\n} } \n",
    "{\"key\":{\n} }\t\n",
    "{\"key\":{\n} }\n\n",
    " {\"key\":{\n} }\n",
    " {\"key\":{\n} } \n",
    " {\"key\":{\n} }\t\n",
    " {\"key\":{\n} }\n\n",
    "\t{\"key\":{\n} }\n",
    "\t{\"key\":{\n} } \n",
    "\t{\"key\":{\n} }\t\n",
    "\t{\"key\":{\n} }\n\n",
    "\t {\"key\":{\n} }\n",
    "\t {\"key\":{\n} } \n",
    "\t {\"key\":{\n} }\t\n",
    "\t {\"key\":{\n} }\n\n",
    "\n{\"key\":{\n} }\n",
    "\n{\"key\":{\n} } \n",
    "\n{\"key\":{\n} }\t\n",
    "\n{\"key\":{\n} }\n\n",
    "\n {\"key\":{\n} }\n",
    "\n {\"key\":{\n} } \n",
    "\n {\"key\":{\n} }\t\n",
    "\n {\"key\":{\n} }\n\n",
    "\n\t{\"key\":{\n} }\n",
    "\n\t{\"key\":{\n} } \n",
    "\n\t{\"key\":{\n} }\t\n",
    "\n\t{\"key\":{\n} }\n\n",
    "\n\t {\"key\":{\n} }\n",
    "\n\t {\"key\":{\n} } \n",
    "\n\t {\"key\":{\n} }\t\n",
    "{\"key\":{\n} \t}",
    "{\"key\":{\n} \t} ",
    "{\"key\":{\n} \t}\t",
    "{\"key\":{\n} \t}\n",
    " {\"key\":{\n} \t}",
    " {\"key\":{\n} \t} ",
    " {\"key\":{\n} \t}\t",
    " {\"key\":{\n} \t}\n",
    "\t{\"key\":{\n} \t}",
    "\t{\"key\":{\n} \t} ",
    "\t{\"key\":{\n} \t}\t",
    "\t{\"key\":{\n} \t}\n",
    "\t {\"key\":{\n} \t}",
    "\t {\"key\":{\n} \t} ",
    "\t {\"key\":{\n} \t}\t",
    "\t {\"key\":{\n} \t}\n",
    "\n{\"key\":{\n} \t}",
    "\n{\"key\":{\n} \t} ",
    "\n{\"key\":{\n} \t}\t",
    "\n{\"key\":{\n} \t}\n",
    "\n {\"key\":{\n} \t}",
    "\n {\"key\":{\n} \t} ",
    "\n {\"key\":{\n} \t}\t",
    "\n {\"key\":{\n} \t}\n",
    "\n\t{\"key\":{\n} \t}",
    "\n\t{\"key\":{\n} \t} ",
    "\n\t{\"key\":{\n} \t}\t",
    "\n\t{\"key\":{\n} \t}\n",
    "\n\t {\"key\":{\n} \t}",
    "\n\t {\"key\":{\n} \t} ",
    "\n\t {\"key\":{\n} \t}\t",
    "{\"key\":{\n} \t}\n",
    "{\"key\":{\n} \t} \n",
    "{\"key\":{\n} \t}\t\n",
    "{\"key\":{\n} \t}\n\n",
    " {\"key\":{\n} \t}\n",
    " {\"key\":{\n} \t} \n",
    " {\"key\":{\n} \t}\t\n",
    " {\"key\":{\n} \t}\n\n",
    "\t{\"key\":{\n} \t}\n",
    "\t{\"key\":{\n} \t} \n",
    "\t{\"key\":{\n} \t}\t\n",
    "\t{\"key\":{\n} \t}\n\n",
    "\t {\"key\":{\n} \t}\n",
    "\t {\"key\":{\n} \t} \n",
    "\t {\"key\":{\n} \t}\t\n",
    "\t {\"key\":{\n} \t}\n\n",
    "\n{\"key\":{\n} \t}\n",
    "\n{\"key\":{\n} \t} \n",
    "\n{\"key\":{\n} \t}\t\n",
    "\n{\"key\":{\n} \t}\n\n",
    "\n {\"key\":{\n} \t}\n",
    "\n {\"key\":{\n} \t} \n",
    "\n {\"key\":{\n} \t}\t\n",
    "\n {\"key\":{\n} \t}\n\n",
    "\n\t{\"key\":{\n} \t}\n",
    "\n\t{\"key\":{\n} \t} \n",
    "\n\t{\"key\":{\n} \t}\t\n",
    "\n\t{\"key\":{\n} \t}\n\n",
    "\n\t {\"key\":{\n} \t}\n",
    "\n\t {\"key\":{\n} \t} \n",
    "\n\t {\"key\":{\n} \t}\t\n",
};


static jtok_parser_t p;
static jtok_tkn_t    tokens[TOKEN_MAX];

int main(void)
{
    printf("\nTesting jtok parser against permutations of empty sub-objects\n");
    unsigned int i;
    unsigned int max_i = sizeof(valid_jsons) / sizeof(*valid_jsons);
    for (unsigned int i = 0; i < max_i; i++)
    {
        p = jtok_new_parser(valid_jsons[i]);
        printf("\n%s ... ", valid_jsons[i]);
        JTOK_PARSE_STATUS_t status = jtok_parse(&p, tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
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
    return 0;
}