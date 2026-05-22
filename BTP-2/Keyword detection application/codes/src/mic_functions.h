#ifndef MIC_FUNCTIONS_H
#define MIC_FUNCTIONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t init_mic(void);
uint16_t read_mic(void);
void sample_audio_int(uint16_t *out, int samp_rate, int seconds);

#ifdef __cplusplus
}
#endif

#endif 