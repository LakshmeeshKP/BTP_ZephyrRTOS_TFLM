/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include "mic_functions.h"

#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys/clock.h>
#include <zephyr/sys/time_units.h>

//refered from zephyr adc_dt example
//safety check to see if zephyr_user is defined and io_channels is mentioned
#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

uint16_t init_mic(void);
uint16_t read_mic(void);
void sample_audio_int(uint16_t *out, int samp_rate, int seconds);

uint32_t delay_time=0;
int err;
uint16_t buf;
struct adc_sequence sequence = {
	.buffer = &buf,
	.buffer_size = sizeof(buf), //size in bytes
};


// setup ADC for mic
uint16_t init_mic(void){
	if (!adc_is_ready_dt(&adc_channels[0])) {
		printk("ADC controller device %s not ready\n", adc_channels[0].dev->name);
		return 0;
	}
	err = adc_channel_setup_dt(&adc_channels[0]);
	if (err < 0) {
		printk("Could not setup channel #%d (%d)\n", 0, err);
		return 0;
	}
	printk("mic init success\n");
	(void)adc_sequence_init_dt(&adc_channels[0], &sequence);
	return 0;
}

// read ADC value corresponding to mic input
uint16_t read_mic(void){
	err = adc_read_dt(&adc_channels[0], &sequence);
	if (err < 0) {
		printk("Could not read (%d)\n", err);
	}
	return buf;
}

//for sampling of audio at 8khz and interpolating to 16khz
void sample_audio_int(uint16_t *out, int samp_rate, int seconds){
	int32_t samp_delay = 1000000/samp_rate;  //us delay between samples (125us delay)
	uint16_t iters=samp_rate*seconds * 2; //16000 samps
	uint32_t period_cyc = k_us_to_cyc_floor32(125);
	uint32_t next_deadline = k_cycle_get_32();
	uint32_t start_cycles = k_cycle_get_32();
	printk("start sampling\n");
	for (uint16_t i=0; i<iters; i=i+2){
		next_deadline += period_cyc;
		out[i] = read_mic();
		while(k_cycle_get_32() < next_deadline){
		}
	}

	//interpolation to match 16khz
	for (uint16_t i=1; i<iters; i=i+2){
		if (i==iters-1) out[i] = out[i-1];
		else out[i] = (uint16_t)((out[i-1]+out[i+1]) >> 1);
	}

	printk("end sampling\n");
}
