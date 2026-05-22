#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/time_units.h>
#include <zephyr/kernel.h>
#include "tflm_functions.h"
#include "model_data_small.hpp"
#include "arm_math.h"


namespace {
	const tflite::Model *model = nullptr;
	tflite::MicroInterpreter *interpreter = nullptr;
	TfLiteTensor *input = nullptr;
	TfLiteTensor *output = nullptr;
	static constexpr int kTensorArenaSize = 9104; //in bytes 
	alignas(16) uint8_t tensor_arena[kTensorArenaSize];
	tflite::MicroMutableOpResolver <7> resolver;
} 

float in_scale, out_scale;
float in_scale_q15;
int32_t in_zp, out_zp;
int8_t *input_buf;
static int idx_stream=0;

void tflm_init(void){
	model = tflite::GetModel(model_data_small);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		MicroPrintf("Model provided is schema version %d not equal "
					"to supported version %d.",
					model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}

	resolver.AddExpandDims();
	resolver.AddConv2D();
	resolver.AddMaxPool2D();
	resolver.AddReshape();
	resolver.AddMean();
	resolver.AddSoftmax();
	resolver.AddFullyConnected();
	
	static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize);
	interpreter = &static_interpreter;
	
	if (interpreter->AllocateTensors() != kTfLiteOk) {
		MicroPrintf("AllocateTensors() failed");
		return;
	}   
	
	input = interpreter->input(0);
	output = interpreter->output(0);
	
	MicroPrintf("Arena used bytes: %d\n", interpreter->arena_used_bytes()); //to optimise arena size
	MicroPrintf("input tensor bytes: %d\n", input->bytes); //debug info
	
	in_scale = input->params.scale, out_scale = output->params.scale;
	in_zp = input->params.zero_point, out_zp = output->params.zero_point;
	
	input_buf = tflite::GetTensorData<int8_t>(input);
	
	MicroPrintf("tflm init success\n");
}

// quantise and insert one frame of MFCC output into tflm input buffer
void insert_input(float frame[256], int DCToutputs){
	for (int j=0; j<DCToutputs; j++){
		int8_t val = (int8_t)(round(frame[j] / in_scale) + in_zp);
		if (val > 127) val = 127;
		if (val < -128) val = -128;
		input_buf[idx_stream++]= val;
	}
}

// perform inference and print best confidence
void perform_inference_stream(void){
	MicroPrintf("invoke status: %d", interpreter->Invoke());
	idx_stream=0; //reset idx for next inference

	float y[8];
	int prediction = -1;
	MicroPrintf("0:'down' 1:'go' 2:'left' 3:'no' 4:'right' 5:'stop' 6:'up' 7:'yes'\n");
	MicroPrintf("predictions:\n");
	for (int i = 0; i<8; i++) {
		y[i] =(tflite::GetTensorData<int8_t>(output)[i] - out_zp) * out_scale;
		MicroPrintf("output %d: %f\n", i, y[i]);
		if (y[i]>0.6f) prediction = i; //cutoff set at 0.6, can be adjusted
	}
	
	switch(prediction){
	case 0:
	MicroPrintf("Detected keyword: down\n");
	break;
	case 1:
	MicroPrintf("Detected keyword: go\n");
	break;		
	case 2:
	MicroPrintf("Detected keyword: left\n");
	break;
	case 3:		
	MicroPrintf("Detected keyword: no\n");
	break;
	case 4:
	MicroPrintf("Detected keyword: right\n");	
	break;
	case 5:
	MicroPrintf("Detected keyword: stop\n");
	break;
	case 6:
	MicroPrintf("Detected keyword: up\n");	
	break;
	case 7:
	MicroPrintf("Detected keyword: yes\n");
	break;
	default:
	MicroPrintf("None\n");
	break;
	}
	return;
}