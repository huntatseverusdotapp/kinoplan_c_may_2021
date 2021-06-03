#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdlib.h>

#define OPERATIONS_NUMBER 15

char *operations[OPERATIONS_NUMBER];
int (*operation_functions[OPERATIONS_NUMBER])(int64_t, int64_t, int64_t *);

enum e_calculation_result {
    ecrOk = 0,
    ecrUnknownOperation = 1,
    ecrArithmeticError = 2,
    ecrInt32Overflow = 3
};

enum e_calculation_result performCalculation(char *operation, int32_t a,
					     int32_t b, int64_t * result);

int plus(int64_t a, int64_t b, int64_t * result);	// 0
int minus(int64_t a, int64_t b, int64_t * result);	// 1
int multiply(int64_t a, int64_t b, int64_t * result);	// 2
int divide(int64_t a, int64_t b, int64_t * result);	// 3
int mod(int64_t a, int64_t b, int64_t * result);	// 4
int inv_plus(int64_t a, int64_t b, int64_t * result);	// 5
int bitwise_and(int64_t a, int64_t b, int64_t * result);	// 6
int bitwise_or(int64_t a, int64_t b, int64_t * result);	// 7
int bitwise_xor(int64_t a, int64_t b, int64_t * result);	// 8
int shift_left(int64_t a, int64_t b, int64_t * result);	// 9
int shift_right(int64_t a, int64_t b, int64_t * result);	// 10
int l_and(int64_t a, int64_t b, int64_t * result);	// 11
int l_or(int64_t a, int64_t b, int64_t * result);	// 12
int divide_plus(int64_t a, int64_t b, int64_t * result);	// 13
int divide_minus(int64_t a, int64_t b, int64_t * result);	// 14

#endif				// OPERATIONS_H
