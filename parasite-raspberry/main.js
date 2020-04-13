const {app, BrowserWindow} = require('electron')

//Cria uma instância de BrowserWindow e carrega o index.html
function createWindow(){
  window = new BrowserWindow({width:800, height:480})

  /*let url = require('url').format({
    protocol: 'file',
    slashes: true,
    pathname: require('path').join(__dirname, 'index.html')
  })
  window.loadURL(url)*/
  window.loadFile("index.html")

  //roda códigos python com o package python-shell
  let {PythonShell} = require('python-shell');

  PythonShell.run('file.py', null,  function  (err, results)  {
   if  (err)  throw err;
   console.log('file.py finished.');
   console.log('results', results);
  });
}

//roda o createWindow() quando a aplicação está pronta
app.on('ready', createWindow)
