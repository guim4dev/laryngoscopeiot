var app = require('electron').remote;
var fs = require('fs');

function getVideo() {
  //seta as variáveis de acordo com as tags da página
  const video = document.getElementById('video');
  const snapshotCanvas = document.getElementById('snapshot');
  const capture = document.getElementById('capture');

  //Pega o vídeo da camera e coloca na tag video
  const constraints = {
    video: {
    width: {min: 1280},
    height: {min: 720},
    aspectRatio: 1.777777778
    }
  };

  navigator.mediaDevices.getUserMedia(constraints).
    then((stream) => {video.srcObject = stream});

  //Salva uma imagem do vídeo
  capture.addEventListener('click', function(){
    snapshot.width = video.videoWidth;
    snapshot.height = video.videoHeight;

    var context = snapshotCanvas.getContext('2d');
    //desenha a imagem do vídeo no canvas
    context.drawImage(video, 0, 0, snapshotCanvas.width, snapshotCanvas.height);
    //salva a imagem do canvas num arquivo png
    const url = snapshotCanvas.toDataURL('image/png',0.8);
    const base64Data = url.replace(/^data:image\/png;base64,/, "");
    fs.writeFile('image.png', base64Data, 'base64', function (err) {
        console.log(err);
    });
  });
}

function errorCallback(e) {
   console.log('Error', e)
}

window.onload = getVideo
