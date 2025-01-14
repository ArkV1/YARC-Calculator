#include "exponent.h"

double power(double base, int exponent) {
    // Handle case when exponent is 0
    if (exponent == 0) {
        return 1.0;
    }
    
    // Handle negative exponent
    if (exponent < 0) {
        base = 1.0 / base;        // Convert to positive by using reciprocal
        exponent = -exponent;     // Make exponent positive
    }
    
    // Multiply base by itself 'exponent' times
    double result = 1.0;
    for (int i = 0; i < exponent; i++) {
        result = result * base;
    }
    
    return result;
}

// double exp(double x) {
//     // TODO: Implement exponential function
//     return 0.0;
// }

// double ln(double x) {
//     // TODO: Implement natural logarithm
//     return 0.0;
// } 