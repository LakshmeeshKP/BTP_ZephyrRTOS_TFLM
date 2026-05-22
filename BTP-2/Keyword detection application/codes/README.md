build and test command:

(activate the zephyr environment)
zephyrproject\.venv\Scripts\activate.bat
cd C:\Users\laksh\zephyrproject\zephyr

(src folder location: zephyrproject/zephyr/applications/final/src)
(overlay for ADC, and cdc-acm-conole snippet included at build)
west build -p always -b rpi_pico_rp2040_w -S cdc-acm-console applications/final -- -DDTC_OVERLAY_FILE=mic_pico_w.overlay

(view serial output using pyserial)
python -m serial.tools.miniterm "COM4" 115200



