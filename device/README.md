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

https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/
https://esp32io.com/tutorials/esp32-force-sensor

#### Sumariozao:

Portas disponiveis para captura de dados:

GPIO 2, GPIO 12, GPIO 13

GPIO 2 - Interruptor
GPIO 12 - Force Sensor 1
GPIO 13 - Force Sensor 2