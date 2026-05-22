#include <zephyr/kernel.h>
#include <zephyr/sys/clock.h>
#include <stdio.h>
#include "arm_math.h"
#include "constants.h"
#include "tflm_functions.h"
#include "mic_functions.h"

arm_status status;
arm_mfcc_instance_f32 mfcc_inst;

uint16_t samples[16000];
float32_t mfcc_input[256]; 
float32_t mfcc_output[256];

void mfcc_init(void);
void get_spectrogram_stream(uint16_t input[16000]);

int main(void){
    k_sleep(K_MSEC(5000));
    init_mic();
    mfcc_init();
    tflm_init();
    while(1){
        printf("recording in 3..\n");
        k_sleep(K_MSEC(1000));
        printf("recording in 2..\n");
        k_sleep(K_MSEC(1000));
        printf("recording in 1..\n");
        k_sleep(K_MSEC(1000));

        //get 1 sec audio samples at 8khz and interpolate to 16khz
        sample_audio_int(samples, 8000, 1);
        k_sleep(K_MSEC(1000));

        uint32_t start_cycles = k_cycle_get_32(); //to measure features computation time
        get_spectrogram_stream(samples); 
	    uint32_t delay_time = k_cyc_to_us_floor32((uint64_t)(k_cycle_get_32() - start_cycles));
	    printk("spectrogram time: %d us\n", delay_time);
        
        printk("start inference\n");
        k_sleep(K_MSEC(1000));

        start_cycles = k_cycle_get_32(); //to measure inference time
        perform_inference_stream();
	    delay_time = k_cyc_to_us_floor32((uint64_t)(k_cycle_get_32() - start_cycles));
	    printk("inference time: %d us\n", delay_time);

        k_sleep(K_MSEC(5000));
    }

    return 0;
}

void mfcc_init(void){
    status = arm_mfcc_init_f32(&mfcc_inst, mfcc_init_fftLen, nbMelFilters, nbDctOutputs, dctCoefs, filterPos, filterLengths, filterCoefs, windowCoefs);
    if (status != ARM_MATH_SUCCESS) {
        printf("init failed!\n");
        while(1){}; 
    }
    printf("mfcc init success\n");
}

void get_spectrogram_stream(uint16_t input[16000]){
    for (int i = 0; i < 125; i++) {
        int base = i * 128;
        for (int j = 0; j < 256; j++) {
            int idx1=base+j;
            mfcc_input[j] = (idx1 < 16000) ? (float)(input[idx1]/32768.0f - 1.0f) : 0.01f; //convert to from uint16 to float
        }
        arm_mfcc_f32(&mfcc_inst, mfcc_input, mfcc_output, temp); 
        for (int k = 0; k < 13; k++) {
            mfcc_output[k] = mfcc_output[k]/1.33f;
        }  
        insert_input(mfcc_output, nbDctOutputs); //quantise input and insert into tflm input buffer
    }
}
