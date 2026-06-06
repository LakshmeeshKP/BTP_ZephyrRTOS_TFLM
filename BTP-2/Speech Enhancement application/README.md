- This contains the documentation report of my attempt to deploy GTCRN on Rpi Pico using TFLM during the BTP.<br>
- The modified python files from the [gtcrn git repo](https://github.com/Xiaobin-Rong/gtcrn) are uploaded to the codes folder<br>
- codes folder contains these files:<br>
gtcrn/stream/modules/convolution.py - modified gtcrn file<br>
gtcrn/stream/gtcrn_stream.py - modified gtcrn file<br>
litert_conv_SliceOutside.py - .tflite converter using LiteRT<br>
quantizer.py - .tflite converter + quantizer using LiteRT<br>

- "tflite files" folder contains these files:<br>
gtcrn_lkp.tflite - result from litert_conv_SliceOutside.py script<br>
gtcrn_lkp_quantized.tflite - result from quantizer.py script (Note: Not successfully full INT8 quantized)<br>