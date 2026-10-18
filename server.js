#!/usr/bin/env node
const net = require("net");
const { exec } = require("child_process");
const fs = require("fs");
const path = require("path");

const server = net.createServer((socket) => {
  console.log("3DS Connected!");

  // Default config fallback
  const customConfigPath = "TDconfig.json";
  let config = {
    buttons: [
      { name: "CMD 1 (UNASSIGNED)", command: "echo 'Slot 1 empty'" },
      { name: "CMD 2 (UNASSIGNED)", command: "echo 'Slot 2 empty'" },
      { name: "CMD 3 (UNASSIGNED)", command: "echo 'Slot 3 empty'" },
      { name: "CMD 4 (UNASSIGNED)", command: "echo 'Slot 4 empty'" },
    ],
  };

  // Read teammate's config file dynamically
  const configPath = path.join(process.cwd(), customConfigPath);
  if (fs.existsSync(configPath)) {
    try {
      const parsed = JSON.parse(fs.readFileSync(configPath, "utf8"));
      if (parsed.buttons) {
        for (let i = 0; i < 4; i++) {
          if (parsed.buttons[i]) config.buttons[i] = parsed.buttons[i];
        }
      }
    } catch (e) {
      console.log("[Mac] Error parsing 3ds-config.json");
    }
  }

  // Helper to perfectly center text for the 3DS UI (26 characters wide)
  const center = (str) => {
    if (!str) str = "";
    const text = str.substring(0, 26);
    const left = Math.floor((26 - text.length) / 2) + text.length;
    return text.padStart(left, " ").padEnd(26, " ");
  };

  // Send the UI labels to the 3DS immediately upon connection
  const uiPayload = `INIT:${center(config.buttons[0].name)}|${center(config.buttons[1].name)}|${center(config.buttons[2].name)}|${center(config.buttons[3].name)}\n`;
  socket.write(uiPayload);

  socket.on("data", (data) => {
    const cmd = data.toString().trim();
    if (cmd.startsWith("INIT")) return; // Ignore echoes

    const runCommand = (commandStr, name) => {
      console.log(`[Mac] Executing ${name}: ${commandStr}`);
      socket.write(`\n--- Running: ${name} ---\n`);

      exec(commandStr, (error, stdout, stderr) => {
        if (stdout) socket.write(stdout);
        if (stderr) socket.write(`Error: ${stderr}\n`);
        if (error) socket.write(`Crash: ${error.message}\n`);
        socket.write(`--- Done ---\n`);
      });
    };

    // Map standard payloads to dynamic config commands
    if (cmd.includes("CMD:0"))
      runCommand(config.buttons[0].command, config.buttons[0].name);
    else if (cmd.includes("CMD:1"))
      runCommand(config.buttons[1].command, config.buttons[1].name);
    else if (cmd.includes("CMD:2"))
      runCommand(config.buttons[2].command, config.buttons[2].name);
    else if (cmd.includes("CMD:3"))
      runCommand(config.buttons[3].command, config.buttons[3].name);
  });

  socket.on("error", (err) => {
    if (err.code === "ECONNRESET") console.log("3DS Disconnected cleanly.");
    else console.log(`Socket Error: ${err.message}`);
  });
});

server.listen(5656, "0.0.0.0", () =>
  console.log("Host server listening on port 5656..."),
);
