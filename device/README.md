# Device do LaringoscópioT

## Desenvolvendo localmente

### Pré-requisitos:


- Uma placa ESP32-CAM
- Um módulos ESP32-CAM-MB ou uma FTDI Programmer (a placa ESP32-CAM não tem porta USB própria)
- Platform IO

Com a placa e o módulo em mãos, é necessário instalar o driver do CH340, de forma que você consiga utilizar a placa MB ou a FTDI Programmer para manipular o programa rodando no ESP32


### Fazendo Upload de código

- Conecte o ESP32-CAM-MB (ou FTDI Programmer) via porta USB no seu computador
- Aperte e segure o botão de BOOT do ESP32-CAM-MB (do lado esquerdo da entrada USB, escrito IOO logo abaixo)
- Aperte e segure o botão de RESET da ESP32-CAM (não é o RST que está no módulo de programação, mas sim o da placa)
- Solte os dois botões simultaneamente
Agora, a placa está em modo de bootloader - onde você vai conseguir fazer o upload o seu código.

- Faça upload utilizando o PlatformIO (podendo ser via extensão do VSCode ou CLI)
- Aperte o botão de RESET na ESP32-CAM (isto é feito para que a placa saia do modo de bootloader)

Pronto! Seu programa está rodando na placa ESP32-CAM :)
Note que esse processo é necessário sempre que quiser subir uma alteração do seu código para a placa.

#### Boas referencias

<!-- PIO, ESP32CAM e outras coisas -->
https://github.com/me-no-dev/ESPAsyncWebServer/#using-platformio
https://microcontrollerslab.com/esp32-server-sent-events-sse-web-server-arduino-ide/#:~:text=Server%2DSent%20Events%20also%20known,changes%20on%20the%20server%20side.
https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/
https://esp32io.com/tutorials/esp32-force-sensor
https://www.espboards.dev/blog/flash-any-esp32-with-esp32-cam-mb/
https://www.tinytronics.nl/en/development-boards/accessories/adapter-boards/esp32-cam-mb-programmer-shield#:~:text=To%20put%20the%20ESP32%2DCAM,the%20code%20can%20be%20uploaded.

<!-- FSR -->
https://esp32io.com/tutorials/esp32-force-sensor
https://learn.adafruit.com/force-sensitive-resistor-fsr/overview

<!-- ESP32CAM - Streaming da Imagem com a Lib Async -->
https://github.com/me-no-dev/ESPAsyncWebServer/issues/908
https://gist.github.com/me-no-dev/d34fba51a8f059ac559bf62002e61aa3
https://randomnerdtutorials.com/esp32-cam-ov2640-camera-settings/
##### Instrucoes Extras para Linux
https://www.reddit.com/r/archlinux/comments/8j53dq/how_do_i_install_ch340_chip_drivers_on_arch/ (primeira thread - instalacao dos drivers CH340)

TL;DR: 
```sh
> uname -r # checar versao do kernel
> sudo pacman -S linux-headers dkms # escolher headers da versao do kernel 
> yay -S i2c-ch341-dkms # instalar de fato o driver apontando para o header do kernel atual
> reboot # necessario reboot para reiniciar a maquina com o novo driver
``` 

#### Sumariozao:

Portas disponiveis para captura de dados:

GPIO 2, GPIO 12, GPIO 13

GPIO 2 - Interruptor
GPIO 12 - Force Sensor 1
GPIO 13 - Force Sensor 2

#### Problema:

GPIOs normalmente disponiveis sao apenas ADC1. Nao rola usar WiFi e fazer analogRead ao mesmo tempo
https://github.com/esphome/issues/issues/1301#issuecomment-768408416
https://forum.arduino.cc/t/how-esp32-cam-adc-analogread/932937
