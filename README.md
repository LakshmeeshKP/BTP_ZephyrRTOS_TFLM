This Github repo contains the documentation of the work done by me during my BTech Project under Prof. Siddharth Tallur (Dept. of Electrical Engineering, IIT Bombay) <br>

Quick highlights to easily navigate the repository: <br>
BTP-1 folder: <br>
- [BTP1_documentation_report](https://github.com/LakshmeeshKP/BTP_ZephyrRTOS_TFLM/blob/main/BTP-1/BTP%20Report/BTP1_documentation_report.pdf) explains setting up Zephyr RTOS for RPi Pico W, basic intro to understanding zephyr's portability and build system, intro to Tensorflow for Microcontrollers (TFLM) and finally the documentation of a voice keyword detection application using TFLM (only BTP-1 phase)
- STFT implementation using KissFFT library (not used in project; CMSIS-DSP was used)
- Jupyter notebook used for training keyword detection model
- Board files used for Rpi Pico W

BTP-2 folder: <br>
- full documentation of the completed keyword detection application 
- codes used to test microphone (MAX9814 module)
- in "Zephyr RTOS exploratory" folder, I have made reports which may help you understand Zephyr portability and DMA porting better. Also includes a Zephyr shell application for streaming from ADC to PC via DMA, and overclocking
- documentation on my attempt to deploy a GTCRN speech enhancement model on Pico
