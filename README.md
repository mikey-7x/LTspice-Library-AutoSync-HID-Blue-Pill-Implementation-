# LTspice Library AutoSync (HID Blue Pill Implementation)

This project provides a comprehensive, automated solution for integrating the popular `thyristr.lib` third-party component library into LTspice.

By default, adding a massive third-party library to LTspice requires manually drawing symbols, linking attributes, and routing SPICE netlist pins for hundreds of individual components. This repository solves that bottleneck. It includes the standard manual instructions for single components, alongside an advanced **STM32 Blue Pill HID (BadUSB)** C++ payload that completely automates the generation of all 186 SCRs, TRIACs, and UJTs in under a minute.

---

## 👨‍🔬 Author Info

**Project Creator:** *Mikey-7x / Yogesh R. Chauhan*  
**GitHub:** [github.com/mikey-7x](https://github.com/mikey-7x)  
**License:** [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)  
**Date:** 29 August 2026  

---

## 🛠️ Features & Debugging Advancements

Through extensive troubleshooting, this automated workflow resolves several critical LTspice and hardware quirks:

*   **Native Geometry Cloning:** Instead of drawing flawed geometric shapes, the automation script dynamically clones the pristine `SCR.asy` and `TRIAC.asy` vector files directly from LTspice's native `Misc` directory.
*   **Custom UJT Generation:** Because LTspice lacks a native generic UJT symbol, the script mathematically constructs a perfect, proportionally accurate N-Type UJT (complete with the solid overhanging base channel and precise emitter arrow).
*   **Regex Pin Routing Override:** LTspice's native default pin order (`A=1, K=2, G=3`) fundamentally conflicts with the standard `thyristr.lib` `.subckt` order (`A=1, G=2, K=3`). The script intercepts the cloned templates and uses Regex to physically rewrite the `SpiceOrder` netlist pins, preventing simulation failures like unintended forward-biasing.
*   **Buffer Flood Protection:** Standard `Keyboard.print()` loops crash the Windows PowerShell buffer. The C++ payload utilizes a custom `sendLine()` chunking delay to ensure the STM32 never drops a character during rapid injection.
*   **Hardware Re-enumeration:** Bypasses the common "USB Not Recognized" error caused by the Blue Pill's R10 resistor by forcing a physical pin state reset (`PA12`) during the boot sequence.

---

## Method 1: The Manual Way (For Single Components)

If you only need to add one specific component (e.g., the `2N1595` SCR), follow these manual steps:

1. **Download and Place the Library File:** 
   Download `thyristr.lib` from [HMGrunthos/InrushTimer](https://github.com/HMGrunthos/InrushTimer/blob/master/thyristr.lib). Press `Win + R`, paste `%localappdata%\LTspice\lib\sub`, press Enter, and move the downloaded file into this folder.
2. **Auto-Generate the Base Symbol:** 
   Open LTspice, go to `File -> Open`, and open `thyristr.lib` (change the file filter to `All Files (*.*)`). Scroll down to the line `.subckt 2N1595 anode gate cathode`, right-click directly on the word `.subckt`, and select **Create Symbol**.
3. **Save to a Custom Folder:** 
   In the save window, click the address bar, paste `%localappdata%\LTspice\lib\sym`, and press Enter. Create a New Folder named `Thyristors`, open it, and click Save.
4. **Open the Master Graphic:** 
   Go to `File -> Open`. Paste `%localappdata%\LTspice\lib\sym\Misc` into the address bar. Change the file filter to `Symbols (*.asy)` and open `scr.asy`.
5. **Configure Symbol Attributes:** 
   Press `Ctrl + A`. Change Prefix to **X**, Value to **2N1595**, and ModelFile to **thyristr.lib**. Click OK.
6. **Fix the Pin Routing:** 
   Right-click the side pin (Gate) and set its Netlist Order to **2**. Right-click the bottom pin (Cathode) and set its Netlist Order to **3**.
7. **Overwrite and Finalize:** 
   Go to `File -> Save As`. Navigate to your `%localappdata%\LTspice\lib\sym\Thyristors` folder. Save the file as `2N1595.asy`, clicking Yes to overwrite the auto-generated rectangle.

---

## Method 2: The Advanced Blue Pill HID Automator (For All 186 Components)

To generate all 186 SCRs, TRIACs, and UJTs instantly with perfect graphics and routing, flash the provided C++ payload to an STM32F103C8T6 (Blue Pill).

### 🚀 Installation & Execution
> **[View the Blue Pill HID Payload Here (hid16.ino)](hid16.ino)**  

1.  **Compile the Code:** Use the Arduino IDE or your Termux `mhex` script to compile the C++ payload into a `.bin` or `.hex` file.
2.  **Flash the Blue Pill:** Flash the firmware using an ST-Link.
3.  **Execute the Automation:**
    *   **CRITICAL:** Unplug the ST-Link from the Blue Pill so it is completely unpowered.
    *   Plug the Blue Pill directly into your Windows PC via the micro-USB port.
    *   The hardware reset (`PA12`) will trigger, Windows will mount the HID keyboard, and the script will automatically execute in PowerShell.

i provideded the pre-compiled hex code of this **LTspice Library AutoSync** code to upload directly in blue pill board and directly attached it to your computer to install all libraries immediately.
> **[View the hex file of Blue Pill HID Payload Here (hid16.hex)](hid16.hex)**

---

## 🧪 Verification: Perfect Phase-Controlled Rectifier Simulation

To verify the installation was successful and prevent simulation calculation errors (like avalanche breakdown or floating ground), build this exact Phase-Controlled Half-Wave Rectifier:

1.  **Place the Component:** Press `F2` and place your new `2N1595` SCR. **Do not rotate or mirror it.** The Anode (triangle) must point UP, and the Cathode (flat line) must point DOWN.
2.  **Wire the Main Loop:** Connect `Mains(+) -> 10Ω Load Resistor -> SCR Anode`. 
3.  **Wire the Trigger Loop:** Connect `Trigger(+) -> 100Ω Gate Resistor -> SCR Gate`.
4.  **Establish Ground:** Connect `Mains(-)`, `Trigger(-)`, and `SCR Cathode` together to a single bottom return wire. **Press `G` and place a Ground symbol on this bottom wire.** *(Without ground, the SPICE engine will fail to calculate).*
5.  **Configure Sources (Safe Limits):**
    *   **Mains:** Right-click -> Advanced -> SINE. `Amplitude = 24`, `Freq = 50`. *(The 2N1595 is a 50V component; using 325V will cause avalanche breakdown).*
    *   **Trigger:** Right-click -> Advanced -> PULSE. `Vinitial = 0`, `Von = 5`, `Tdelay = 5m`, `Trise = 1u`, `Tfall = 1u`, `Ton = 1m`, `Tperiod = 20m`.
6.  **Simulate:** Click `.op` to add the text `.tran 100m`, hit Run.
7.  **Probe the Load:** Hover over the 10Ω Load Resistor until the cursor turns into a red current clamp and click. You will see the perfectly chopped half-wave output firing at exactly 5ms.

---

## 📜 License
This project is open-source and available under the [MIT License](LICENSE).

---

## **📜 Credits**  
Developed with  ❤️ by **[mikey-7x](https://github.com/mikey-7x)** 🚀🔥  


[other repository](https://github.com/mikey-7x?tab=repositories)

Special thanks to the open-source community for providing awesome libraries!

