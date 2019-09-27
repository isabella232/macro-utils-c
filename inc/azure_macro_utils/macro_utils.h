// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MACRO_UTILS_H
#define MACRO_UTILS_H

#include "azure_macro_utils/macro_utils_generated.h"

#ifdef __cplusplus
#include <cstring>
#include <cstddef>
extern "C" {
#else
#include <string.h>
#include <stddef.h>
#endif

#if (defined OPTIMIZE_RETURN_CODES)
    #define MU_FAILURE 1
#else
    #define MU_FAILURE __LINE__
#endif

/*"pointer or NULL" macro - because when printf-ing arguments NULL is not valid for %s (section 7.1.4 of C11 standard) */
#define MU_P_OR_NULL(p) (((p)!=NULL)?(p):"NULL")
#define MU_WP_OR_NULL(p) (((p)!=NULL)?(p):L"NULL")

#define MU_TOSTRING_(x) #x
#define MU_TOSTRING(x) MU_TOSTRING_(x)

#define MU_TRIGGER_PARENTHESIS(...) ,

#define MU_LPAREN (

#define MU_C2_(x,y) x##y

#define MU_C2(x,y) MU_C2_(x,y)

#define MU_C3(x,y,z) MU_C2(x, MU_C2(y,z))

#define MU_C4(x,y,z, u) MU_C2(MU_C2(x,y), MU_C2(z,u))

#define MU_C5(x,y,z,u, v) MU_C2(MU_C4(x,y, z, u), v)

#define MU_C1_(x) x

#define MU_C1(x) MU_C1_(x)

#define MU_C2STRING(x,y) x y

#define MU_C3STRING(x,y,z) x y z

#define MU_C4STRING(x,y,z,u) x y z u

#define MU_C5STRING(x,y,z,u,v) x y z u v

/* we need some sort of macro that does:
MU_IF(0, "true", "false") => "false"
MU_IF(1, "true", "false") => "true"
MU_IF(X, "true", "false") => "true"
*/

#define MU_INTERNALIF(x) MU_INTERNALIF##x
#define MU_INTERNALIF0

#define MU_ISZERO(x) MU_COUNT_ARG(MU_INTERNALIF(x))

#define MU_IF(condition, trueBranch, falseBranch) MU_C2(MU_IF,MU_ISZERO(condition))(trueBranch, falseBranch)
#define MU_IF0(trueBranch, falseBranch) falseBranch
#define MU_IF1(trueBranch, falseBranch) trueBranch


/*the following macro want to eat empty arguments from a list */
/*examples:                                                   */
/*MU_EAT_EMPTY_ARGS(, , X) expands to X                       */

#define MU_EXPAND_TO_NOTHING(arg)
#define MU_EAT_EMPTY_ARG(arg_count, arg) MU_IF(MU_ISEMPTY(arg),,arg) MU_IF(MU_ISEMPTY(arg),MU_EXPAND_TO_NOTHING,MU_IFCOMMALOGIC)(MU_DEC(arg_count))
#define MU_EAT_EMPTY_ARGS(...) MU_FOR_EACH_1_COUNTED(MU_EAT_EMPTY_ARG, __VA_ARGS__)

#define MU_EMPTY()
#define MACRO_UTILS_DELAY(id) id MU_EMPTY MU_LPAREN )

//
//
// MU_DEFINE_ENUM
//
//

// This macro can handle enumeration-constants being constant expressions.
// Given the following enum:
//
//
// #define TEST_UBER_ENUM_VALUES \
//      test_uber_a, \
//      test_uber_b = 42 \
// 
// MU_DEFINE_ENUM(TEST_UBER_ENUM, TEST_UBER_ENUM_VALUES)
//
// The ToString expands to something like the below given an enum
//
// const char* TEST_UBER_ENUM_ToString(TEST_UBER_ENUM value)
// {
//     static TEST_UBER_ENUM my_test_uber_a;
//     static TEST_UBER_ENUM my_test_uber_b = 42;
//     static int enum_last_value = -1;
//     int id_has_equals;
//     id_has_equals = (my_test_uber_a - 1) == (my_test_uber_a);
//     enum_last_value = id_has_equals ? (my_test_uber_a) : (enum_last_value + 1);
//     if (enum_last_value == value)
//     {
//         return "test_uber_a";
//     }
//     id_has_equals = (my_test_uber_b = 42 - 1) == (my_test_uber_b = 42);
//     enum_last_value = id_has_equals ? (my_test_uber_b = 42) : (enum_last_value + 1);
//     if (enum_last_value == value)
//     {
//         return "test_uber_b = 42";
//     }
//     return "NULL";
// }

#define MU_DEFINE_ENUMERATION_CONSTANT(x) x,

#define MU_INTERNAL_DEFINE_ENUM_VAR(enumName, enumValue) \
    static enumName MU_C2(my_, enumValue);

#define MU_INTERNAL_ASSIGN_ENUM_VALUE(enumValue) \
    id_has_equals = (MU_C2(my_, enumValue) - 1) == (MU_C2(my_, enumValue)); \
    enum_last_value = id_has_equals ? (MU_C2(my_, enumValue)) : (enum_last_value + 1); \
    if (enum_last_value == value) \
    { \
        return MU_TOSTRING(enumValue); \
    }

#define MU_DECLARE_ENUM_TO_STRING(enumName, ...) \
    const char* MU_C3(MU_, enumName, _ToString)(enumName value);

#define MU_DEFINE_ENUM_WITHOUT_INVALID(enumName, ...) \
    typedef enum MU_C2(enumName, _TAG) { MU_FOR_EACH_1(MU_DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; \
    MU_DECLARE_ENUM_TO_STRING(enumName, __VA_ARGS__)

#define MU_DEFINE_ENUM(enumName, ...) \
    MU_DEFINE_ENUM_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), __VA_ARGS__) \

#define MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(enumName, ...) \
    const char* MU_C3(MU_, enumName, _ToString)(enumName value) \
    { \
        MU_FOR_EACH_1_KEEP_1(MU_INTERNAL_DEFINE_ENUM_VAR, enumName, __VA_ARGS__) \
        int enum_last_value = -1; \
        int id_has_equals; \
        MU_FOR_EACH_1(MU_INTERNAL_ASSIGN_ENUM_VALUE, __VA_ARGS__) \
        return "NULL"; \
    }

// this macro is a wrapper on top of MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID, adding an _INVALID value as the first enum value in the enum
#define MU_DEFINE_ENUM_STRINGS(enumName, ...) \
    MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), __VA_ARGS__)

#define MU_ENUM_TO_STRING(enumName, value) \
    MU_C3(MU_, enumName, _ToString)(value)

//
//
// MU_DEFINE_LOCAL_ENUM
//
//

#define MU_DEFINE_ENUMERATION_CONSTANT_AS_STRING(enumValue) \
    MU_TOSTRING(enumValue),

#define MU_DEFINE_LOCAL_ENUM_WITHOUT_INVALID(enumName, ...) \
    typedef enum MU_C2(enumName, _TAG) { MU_FOR_EACH_1(MU_DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; \
    static const char* MU_C3(MU_, enumName, _ToString)(enumName value) \
    { \
        MU_FOR_EACH_1_KEEP_1(MU_INTERNAL_DEFINE_ENUM_VAR, enumName, __VA_ARGS__) \
        int enum_last_value = -1; \
        int id_has_equals; \
        MU_FOR_EACH_1(MU_INTERNAL_ASSIGN_ENUM_VALUE, __VA_ARGS__) \
        return "NULL"; \
    }

// this macro is a wrapper on top of MU_DEFINE_LOCAL_ENUM_WITHOUT_INVALID, adding an _INVALID value as the first enum value in the enum
#define MU_DEFINE_LOCAL_ENUM(enumName, ...) \
    MU_DEFINE_LOCAL_ENUM_WITHOUT_INVALID(enumName, MU_C2(enumName, _INVALID), __VA_ARGS__)

// this macro returns the number of enum values (taking into account that an invalid value is generated)
#define MU_ENUM_VALUE_COUNT(...) (MU_INC(MU_COUNT_ARG(__VA_ARGS__)))

// this macro returns the number of enum values (taking into account that no invalid value is generated)
#define MU_ENUM_VALUE_COUNT_WITHOUT_INVALID(...) MU_COUNT_ARG(__VA_ARGS__)

//
//
// MU_DEFINE_STRUCT
//
//

#define MU_DEFINE_STRUCT_FIELD(fieldType, fieldName) fieldType fieldName;

/*MU_DEFINE_STRUCT allows creating a struct typedef based on a list of fields*/
#define MU_DEFINE_STRUCT(structName, ...) typedef struct MU_C2(structName, _TAG) { MU_FOR_EACH_2(MU_DEFINE_STRUCT_FIELD, __VA_ARGS__)} structName;

//
//
// MU_COUNT_ARRAY_ITEMS
//
//

// this macro allows counting of elements in an array
#define MU_COUNT_ARRAY_ITEMS(A) (sizeof(A)/sizeof((A)[0]))

#ifdef __cplusplus
}
#endif

#endif /*MACRO_UTILS_H*/
