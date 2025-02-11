const { app, BrowserWindow } = require("electron");

const isDev = process.env.NODE_ENV === "development";

let mainWindow;

app.whenReady().then(() => {
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: true,
    },
  });



  mainWindow.loadURL("http://localhost:5173");
});
