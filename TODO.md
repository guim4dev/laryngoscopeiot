# TODOs

- Camera should run solely on core 0, as the WebServer and the sensors are running on core 1. How to split this correctly ?
- Camera needs to stream JPEG. Ideas: different AsyncWebserver + Custom Implementation, different lib (use esp normal WebServer)
- Apply workaround for capturing Force Sensors Input with ADC