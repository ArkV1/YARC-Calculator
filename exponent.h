#ifndef EXPONENT_H
#define EXPONENT_H

/**
 * @brief Calculates x raised to the power of n (x^n)
 * @param x The base number
 * @param n The exponent (power)
 * @return The result of x^n
 */
double power(double x, int n);

/**
 * @brief Calculates e (Euler's number) raised to the power of x (e^x)
 * @param x The exponent
 * @return The result of e^x
 */
double exp(double x);

/**
 * @brief Calculates the natural logarithm of x (ln(x))
 * @param x The input value (must be positive)
 * @return The natural logarithm of x
 */
double ln(double x);

#endif /* EXPONENT_H */ 