#ifndef M_UTILS_H
#define M_UTILS_H
#include "ui.h"

/* Clamp float value between min and max */
float clamp(float num, float min, float max);

/* Generate random values for developing */
void generateRandomValues(ViewModel* viewModel);

/* Generate known values for developing. */
void generateValues(ViewModel* viewModel);

/* Move arr elements 1 to left and override last index. For floats.*/
void insertLast_F(float* arr, int arrLen, float value);


#endif