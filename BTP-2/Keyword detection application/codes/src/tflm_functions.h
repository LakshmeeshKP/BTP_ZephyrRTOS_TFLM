#ifndef TFLM_FUNCTIONS_H
#define TFLM_FUNCTIONS_H

#include <stdint.h>
#include "arm_math.h"

#ifdef __cplusplus
extern "C" {
#endif

void tflm_init(void);
void insert_input(float frame[256], int DCToutputs);
void perform_inference_stream(void);

#ifdef __cplusplus
}
#endif

#endif