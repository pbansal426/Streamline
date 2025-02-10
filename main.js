const { app, BrowserWindow } = require("electron");
const path = require("path");
const net = require("net");

let mainWindow;

// Function to find the running Vite port
const getVitePort = () => {
  return new Promise((resolve) => {
    const defaultPort = 5173; // Default Vite port
    const portRange = [5173, 5180]; // Adjust if needed

    const checkPort = (portIndex = 0) => {
      if (portIndex >= portRange.length) {
        resolve(defaultPort);
        return;
      }

      const port = portRange[portIndex];
      const client = new net.Socket();

      client.once("connect", () => {
        client.destroy();
        resolve(port);
      });

      client.once("error", () => {
        client.destroy();
        checkPort(portIndex + 1);
      });

      client.connect(port, "localhost");
    };

    checkPort();
  });
};

// Create Electron window
const createWindow = async () => {
  const vitePort = await getVitePort();
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: true,
    },
  });

  const viteURL = `http://localhost:${vitePort}`;
  console.log(`Loading Vite frontend from: ${viteURL}`);

  mainWindow.loadURL(viteURL);
};

app.whenReady().then(createWindow);

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});
