# PY32Duino

PY32Duino is a community-driven Arduino framework intended to support the Arm-Cortex series MCUs from Puya Semiconductor.

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

https://regsens.com/package_py32_index.json

This is the new Arduino Core package index for PY32Duino and must be used instead of any previous/old board JSON files.

Example (Arduino IDE):
1. Open Arduino IDE → File → Preferences.
2. In "Additional Boards Manager URLs" paste:
   https://regsens.com/package_py32_index.json
3. Open Tools → Board → Boards Manager, search for "Arduino PY32" and install the PY32Duino core.


## License

(Include your project license here — e.g., MIT, Apache-2.0 — if applicable.)
