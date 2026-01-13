# PY32Duino

PY32Duino is a community-driven Arduino framework intended to support the Arm-Cortex series MCUs from Puya Semiconductor.

## Quick Install (Arduino IDE)

Copy this Boards Manager URL:

```text
https://regsens.com/package_py32_index.json
```

Then in Arduino IDE:

1. **File → Preferences**
2. Paste the URL into **Additional Boards Manager URLs**
3. **Tools → Board → Boards Manager** → search for **Arduino PY32** → **Install**

## Documentation & Getting Started

You can view the official PY32Duino.

## Supported MCUs

Currently we support the following MCUs:

| MCU series   | Support status | Detailed link |
| :----------: | :------------: | :-----------: |
| PY32F002A    | 🔨 (in progress) |               |
| PY32F003     | 🔨 (in progress) |               |
| PY32F030     | 🔨 (in progress) |               |

For more information about development boards, please check the board-specific pages in the documentation above.

## Arduino Boards Manager — Use this new core

Do NOT use the old core board JSON. Please add the following Package Index URL to your Arduino IDE (Preferences → Additional Boards Manager URLs) or to your platform configuration:

```text
https://regsens.com/package_py32_index.json
```

This is the new Arduino Core package index for PY32Duino and must be used instead of any previous/old board JSON files.

Example (Arduino IDE):
1. Open Arduino IDE → File → Preferences.
2. In "Additional Boards Manager URLs" paste:
   https://regsens.com/package_py32_index.json
3. Open Tools → Board → Boards Manager, search for "Arduino PY32" and install the PY32Duino core.

## Programming / Upload Tools

This core can be used with a variety of programming/debug adapters and tools (depending on your target board and how it exposes SWD/UART pins), for example:

- **ST-Link V2** (SWD)
- **WCH-Link** ("WCH-Link Download Debugger", SWD)
- Other compatible SWD/UART programmers supported by your workflow

> Tip: the exact upload method can vary per board/variant. If one tool doesn’t fit your hardware, you can typically switch to another programmer without changing your sketches.

### Host OS support

The bundled tooling in this core is provided for:

- Windows (x64)
- Linux (x64)
- Linux (ARM64 / aarch64)

Linux ARM (32-bit / `arm-linux-gnueabihf`) support is planned in the near future.

## PF2 (NRST) as GPIO + Restore to RESET

On some supported PY32 parts, **PF2 is multiplexed with NRST** (reset). PY32Duino provides a convenient runtime way to switch this using `pinMode()`.

### Use PF2 as GPIO (automatic)

Call `pinMode(PF2, ...)` with a normal GPIO mode (e.g. `OUTPUT`, `INPUT`, `INPUT_PULLUP`, `INPUT_PULLDOWN`, `OUTPUT_OPEN_DRAIN`).

```cpp
pinMode(PF2, OUTPUT);
digitalWrite(PF2, HIGH);
```

Notes:

- Internally, the core updates the **NRST option byte** to GPIO mode when you configure PF2 as a digital pin.
- This operation may trigger a reset/relaunch of option bytes (so don’t do this repeatedly in a loop).
- `pinMode(PF2, INPUT_ANALOG)` intentionally does **not** switch NRST into GPIO mode.

> **Warning (avoid lockout):** If you switch **PF2/NRST** to GPIO you may lose the external reset pin. If you also repurpose/disable your **SWD** pins or debug access at the same time, you can end up with **no reset + no debug connection**, and the MCU can become effectively **unprogrammable** with normal tools.
>
> Recommended safety approach: keep SWD available while experimenting, and always have a “rescue” path in firmware that can restore reset mode (for example: on boot, check a button/jumper/timeout and then call `pinMode(PF2, NRST)` to restore the reset pin).

### Restore PF2 back to RESET (NRST)

When you want PF2 to behave as the hardware reset pin again, do:

```cpp
pinMode(PF2, NRST);
```

After restoring NRST mode, PF2 is no longer available as a normal GPIO pin.


## License

(Include your project license here — e.g., MIT, Apache-2.0 — if applicable.)
