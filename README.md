# ARMA_ESP_SD
This project sends an array of 4800 half-words from Teensy to ESP12s via Serial.
It also saves the array in the SD card (test.csv) via SPI.

# Procedure to upload code on ESP. 
1. The Serial Pins of the ESP are shorted to the Auxilliary GPIO 13, 14 as referred to in the schematic.
2. Connect these to the PC via a USB-Serial converter.
3. Restart the ESP in programming mode by doing the following:
   -In the setup section of Teensy_SendData_SD.ino, comment "normal mode reset RED LED" and uncomment "programming mode GREEN LED"
   -Upload Teensy_SendData_SD.ino to Teensy to start ESP in programming mode.
4. Upload esp_HTTPS_POST_JSON to ESP.
5. Undo step 3 and upload to Teensy, this will start the ESP in normal mode and code uploaded in step 4 will be executed

# esp_HTTPS_POST_JSON
This code receives the serail data sent by the Teensy, converts it to a json object row by row (4800 values in 80 rows of 60 values each ) and sends an HTTPS POST request to the following web server (script copy available in webserver.txt):

https://script.google.com/home/projects/1niV1LXucPBpqVuCeT5znO4vxeuefeQiyRMsjxeojs_2zOYTCKim4Cw6y/edit

which then populates the following Google Sheet:

> https://docs.google.com/spreadsheets/d/1k1_q2rbJBnIklAOPhq4ioqJP_qrIRZhz4bVtBlZTPAw/edit#gid=0

If the connection to the server fails, make sure the SHA 1 fingerprint of the server (Ctl+Shift+I in CHrome browser) match with that specified in esp_HTTPS_POST_JSON.

