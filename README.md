# LED Pattern Player

The **LED Pattern Player** is an efficient microcontroller firmware project developed for **Infineon PSoC boards** using **ModusToolbox within Visual Studio Code**. Engineered for robust performance and clean modular architecture, this project delivers dynamic lighting control tailored for embedded systems development.

## Features

At the core of the firmware is a streamlined control loop that allows users to easily cycle through five distinct operational patterns:
* **Off:** Completely deactivates the LEDs to conserve power and reset the visual display.
* **Slow Blink:** Produces a gentle, rhythmic pulse suited for ambient illumination or standby indicators.
* **Fast Blink:** Delivers a high-frequency flash to command immediate attention or signify active alerts.
* **Double Blink:** Employs a signature two-pulse staccato rhythm that adds a modern, distinct aesthetic touch.
* **Heartbeat:** Mimics a natural biological pulse with a smooth double-beat rhythm, offering an organic and calming visual effect.

## Tech Stack & Tools

* **Hardware:** Infineon PSoC Board
* **IDE / Toolchain:** ModusToolbox integrated within Visual Studio Code (VS Code)
* **Language:** C / C++

## Getting Started

1. **Clone the Repository:**
   ```bash
   git clone [https://github.com/AnishRao2006/LED_pattern_player.git](https://github.com/AnishRao2006/LED_pattern_player.git)

2. **Open in ModusToolbox / VS Code:**
   * Launch VS Code and open the cloned project workspace (LED_pattern_player).
   * Ensure the ModusToolbox extensions and build tools are properly configured in your environment.

3. **Build and Program:**
   * Build the project using the ModusToolbox build tools or Makefile.
   * Flash the compiled firmware onto your target PSoC board.
