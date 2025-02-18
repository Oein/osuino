import {BrowserWindow, app,ipcMain} from "electron";
import logger from "./logger";

// console.log = logger.log.bind(logger);
// console.info = logger.info.bind(logger);
// console.error = logger.error.bind(logger);

app.whenReady().then(() => {
    const win = new BrowserWindow({
        width: 320,
        height: 480,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false, // Required for ipcRenderer
        },
        titleBarStyle: "hidden"
    });

    win.loadURL("http://localhost:5500/hello.html");

    // ipcMain.on('log-message', (event, message) => {
    //     logger.log(message);
    // });
});