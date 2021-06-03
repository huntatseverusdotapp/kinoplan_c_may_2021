#include "operations.h"

#include <stdint.h>
#include <string.h>

char* operations[OPERATIONS_NUMBER] = {
    "+",
    "-",
    "*",
    "/",
    "%",
    "~+",
    "&",
    "|",
    "^",
    "<<",
    ">>",
    "&&",
    "||",
    "/+",
    "/-"
};

int (*operation_functions[OPERATIONS_NUMBER])(int64_t, int64_t, int64_t*) = {
    plus,
    minus,
    multiply,
    divide,
    mod,
    inv_plus,
    bitwise_and,
    bitwise_or,
    bitwise_xor,
    shift_left,
    shift_right,
    l_and,
    l_or,
    divide_plus,
    divide_minus
};

enum e_calculation_result performCalculation(char *operation, int32_t a, int32_t b, int64_t *result) {
    int index = -1;
    for (int i=0; i < OPERATIONS_NUMBER; ++i) {
        if (strcmp(operation, operations[i]) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1)
        return ecrUnknownOperation;
    int64_t a64 = (int64_t) a;
    int64_t b64 = (int64_t) b;
    if (operation_functions[index](a64, b64, result)) {
        if (*result < INT32_MIN || *result > INT32_MAX)
            return ecrInt32Overflow;
        return ecrOk;
    } else {
        return ecrArithmeticError;
    }
}

int plus(int64_t a, int64_t b, int64_t *result) {
    *result = a+b;
    return 1;
}

int minus(int64_t a, int64_t b, int64_t *result) {
    *result = a-b;
    return 1;
}

int multiply(int64_t a, int64_t b, int64_t *result) {
    *result = a*b;
    return 1;
}

int divide(int64_t a, int64_t b, int64_t *result) {
    if (b == 0)
        return 0;
    *result = a/b;
    return 1;
}

int mod(int64_t a, int64_t b, int64_t *result) {
    *result = a%b;
    return 1;
}

int inv_plus(int64_t a, int64_t b, int64_t *result) {
    *result = ~a+~b;
    return 1;
}

int bitwise_and(int64_t a, int64_t b, int64_t *result) {
    *result = a&b;
    return 1;
}

int bitwise_or(int64_t a, int64_t b, int64_t *result) {
    *result = a|b;
    return 1;
}

int bitwise_xor(int64_t a, int64_t b, int64_t *result) {
    *result = a^b;
    return 1;
}

int shift_left(int64_t a, int64_t b, int64_t *result) {
    *result = a<<b;
    return 1;
}

int shift_right(int64_t a, int64_t b, int64_t *result) {
    *result = a>>b;
    return 1;
}

int l_and(int64_t a, int64_t b, int64_t *result) {
    *result = a&&b;
    return 1;
}

int l_or(int64_t a, int64_t b, int64_t *result) {
    *result = a||b;
    return 1;
}

int divide_plus(int64_t a, int64_t b, int64_t *result) {
    if (b == 0)
        return 0;
    *result = a/b+b;
    return 1;
}

int divide_minus(int64_t a, int64_t b, int64_t *result) {
    if (b == 0)
        return 0;
    *result = a/b-b;
    return 1;
}
