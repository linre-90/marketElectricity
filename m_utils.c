#include <stdlib.h>
#include <time.h>
#include "m_utils.h"
#include "ui.h"


float clamp(float num, float min, float max) {
    const float t = num < min ? min : num;
    return t > max ? max : t;
}

void generateRandomValues(ViewModel* viewModel) {
    float eurMax = 3.0f;

    for (int i = 0; i < 24; i++)
    {
        float x = (float)rand() / (float)(RAND_MAX / eurMax);
        viewModel->history[i] = x;
    }
}

void generateValues(ViewModel* viewModel) {
    for (int i = 0; i < 24; i++)
    {
        viewModel->history[i] = i * 0.01f;
    }
}



void insertLast_F(float* arr, int arrLen, float value) {
    for (int i = 1; i < arrLen; i++)
    {
        arr[i - 1] = arr[i];
    }
    arr[arrLen - 1] = value;
}
