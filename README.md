# TerminalDeck 🎮💻

**Turn your modded Nintendo 3DS into a Wi-Fi connected, dual-screen terminal companion deck.**

TerminalDeck bridges retro hardware with modern development workflows. It acts as a physical, touch-enabled macro pad and remote system monitor for your Mac or PC, executing complex build scripts and streaming live terminal outputs directly to your console screens over local Wi-Fi.

---

## ✨ Features
* **Dual-Screen UI:** Interactive touch buttons on the bottom screen, with live `stdout`/`stderr` streaming to the top screen.
* **Universal Compatibility:** Controls any local project (React, Node, Python, C++, etc.) using standard shell commands.
* **Dynamic Configuration:** Reads a local `TDconfig.json` file on the fly to instantly populate the 3DS UI with project-specific commands.
* **Zero-Latency Execution:** Built on raw TCP sockets for instant command execution and log streaming.

---

## 🏗️ Architecture
TerminalDeck operates on a lightweight Client/Server architecture:
1. **The Host Agent (Node.js):** A global CLI tool (`terminal-deck`) running on the host machine. It spins up a TCP socket server, reads the local project's config file, and uses Node's `child_process` to execute shell commands.
2. **The 3DS Client (C / libctru):** A custom homebrew application that paints an ANSI-colored touch interface, sends execution payloads over Wi-Fi, and renders incoming server logs to the top screen using non-blocking sockets.

---

## 🚀 Quick Start

### 1. Host Setup (Mac/Linux/Windows)
Ensure you have Node.js installed. Clone this repository and install the host agent globally:

```bash
git clone [https://github.com/VanshAnand1/terminal-deck.git](https://github.com/VanshAnand1/terminal-deck.git)
cd terminal-deck
sudo npm install -g .
```

### 2. Client Setup (Nintendo 3DS)
1. Download the `TerminalDeck.3dsx` file from the **Releases** tab.
2. Copy the file into the `3ds/` folder on your console's SD card.
3. Launch the app via the Homebrew Launcher. 

For this prototype, the host IP address is hardcoded into `main.c` to bypass Wi-Fi isolation. To compile for a new network, update the IP in `source/main.c` and build using the devkitARM Docker container.

---

## ⚙️ Configuration
TerminalDeck adapts to whatever project you are currently working on. 

In the root directory of your project, create a `TDconfig.json` file. Define up to 4 custom buttons with the exact names you want displayed on the 3DS screens, along with the shell commands they should trigger.

For example: 

```json
{
    "buttons": [
        { "name": "BUILD PROJECT", "command": "npm run build" },
        { "name": "START SERVER", "command": "npm start" },
        { "name": "RUN TESTS", "command": "npm test" },
        { "name": "SYSTEM DIAGNOSTICS", "command": "top -l 1 | head -n 12" }
    ]
}
```

### Running the Agent
Navigate to your project folder containing the config file and start the agent:
```bash
cd path/to/your/project
terminal-deck
```
The moment the 3DS connects, the host server intercepts the connection, parses the JSON config file, and beams the custom UI directly to your console.

---

## 🛠️ Tech Stack
* **3DS Client:** C, libctru, devkitARM toolchain
* **Host Server:** Node.js, TCP Sockets, `child_process` execution
* **Build Environment:** Docker (devkitpro/devkitarm container), Make
