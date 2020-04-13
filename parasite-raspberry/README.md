# Parasite Watch - Raspberry Pi

## Sobre

Esse repositório contém os arquivos de desenvolvimento do Parasite Watch para a plataforma
Raspberry Pi. O Parasite Watch é um projeto desenvolvido em uma parceria da COPPE/LENS com a
Fiocruz e é de propriedade do LENS.

## Instalação

### Instalar o Node.js e npm

Para instalar o Node.js no Raspberry Pi, siga os seguintes passos:

```bash
cd /tmp
wget https://nodejs.org/dist/v10.15.1/node-v10.15.1-linux-armv7l.tar.xz
tar xfv node-v10.15.1-linux-armv7l.tar.xz
cd node-v10.15.1-linux-armv7l
sudo cp -R * /usr/local/
```

Para instalar o npm:
```bash
sudo apt-get install -y npm
sudo npm install -g npm
```

você pode checar suas versões do node.js rodando
```bash
node -v
```

e do npm rodando
```bash
npm -v
```

### Clonar o repositório

Para clonar o repositório, use o seguinte comando:
```bash
git clone https://www.mrdevops-gitlab.com/ParasiteWatch/parasite-raspberry.git
```
Você precisará usar suas credenciais do GitLab do LENS.
Entre na pasta do repositório usando
```bash
cd parasite-raspberry
```

### Instalar o Electron

A versão mais atual suportada pelo RPi do Electron é a 3.0.13. Para instalar, use na pasta do projeto
```bash
npm install electron@3.0.13 --save-dev
```

### Ativar a Pi Camera como Webcam

Para ativar o uso da Pi Camera como uma Webcam, use:
```bash
sudo modprobe bcm2835-v4l2
```

### Rodar o projeto

Para rodar o projeto, basta usar
```bash
npm start
```
