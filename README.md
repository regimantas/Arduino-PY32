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


## Comparator (experimental)

This core includes an Arduino-style `Comparator` class.

**Quick example**

```cpp
Comparator comp1(1);

void onComp() {
   // comparator edge event
}

void setup() {
   // PA1 is a valid COMP1 (+) input on PY32F002A and PY32F003.
   // COMP1 (+): PB8 / PB2 / PA1
   // COMP1 (-): PB1 / PA0 OR internal sources (VREF/VCC/TS/...)
   // 3rd arg is optional output *mirror* pin (software-driven).
   // You can pass NC, or simply omit the 3rd argument.
   bool ok = comp1.begin(PA1, VREF_1_2, NC);

   comp1.hysteresis(mV20);
   comp1.filter(Samples8);
   comp1.power(HighSpeed);
   comp1.attachInterrupt(onComp, Rising);
}

void loop() {
   bool level = comp1.read();
}
```

**Internal minus sources**
- `VREF`, `VREF_1_4`, `VREF_1_2`, `VREF_3_4`, `VCC`, `TS`

Notes:
- These sources are used as the **minus** input (2nd argument to `begin(...)`).
- The **PY32F002A and PY32F003 datasheets list the same internal sources**: `VCC`, temperature sensor output (`TS`), and `VREFINT` plus divider fractions (1/4, 1/2, 3/4).
- Quick examples:

```cpp
comp1.begin(PA1, VREF_1_2, NC);   // compare PA1 vs 1/2 VrefInt
comp1.begin(PA1, VCC, NC);        // compare PA1 vs VCC
comp1.begin(PA1, TS, NC);         // compare PA1 vs temperature sensor
```

**PY32F002A: Comparator external pins (supported by this core)**
- COMP1 (+): PA1, PB2
- COMP1 (-): PA0, PB1 or internal minus sources (`VREF_*`, `VCC`, `TS`)
- COMP2 (+): PA3, PB6
- COMP2 (-): PA2, PB3 or internal minus sources (`VREF_*`, `VCC`, `TS`)

**PY32F003: Comparator external pins (supported by this core)**
- COMP1 (+): PA1, PB2, PB8
- COMP1 (-): PA0, PB1 or internal minus sources (`VREF_*`, `VCC`, `TS`)
- COMP2 (+): PA3, PB4, PB6, PF3
- COMP2 (-): PA2, PB3, PB7 or internal minus sources (`VREF_*`, `VCC`, `TS`)

Notes:
- These are the pin combinations accepted by `begin(...)` in this core.
- Whether a pin is physically available depends on the exact package/board.
- If `begin(...)` returns `false`, that pin combination is not supported (or the pin symbol isn't available for your selected package).

**API (what each parameter means)**

`bool begin(plusPin, minusPinOrInternal, outPin)`
- `plusPin`: comparator non-inverting input pin
- `minusPinOrInternal`: comparator inverting input pin **or** one of the internal sources listed above
- `outPin`: optional “mirror” GPIO output (pass `NC` to disable). This is **not** a true peripheral output.
   It updates when you call `read()` and when comparator interrupts fire (after `attachInterrupt()`).
   Don’t also `digitalWrite()` the same pin from your sketch.
- Returns `true` on success (valid pin mapping + peripheral started)

`void hysteresis(value)`
- `mV0` disables hysteresis
- `mV20` enables hysteresis

`void filter(samples)`
- Digital filter sample count
- `Samples0` disables filter
- Example presets: `Samples1`, `Samples2`, `Samples4`, `Samples8`, `Samples16`

`void power(mode)`
- `HighSpeed` (fastest)
- `MediumSpeed` (lower power)

`void attachInterrupt(callback, mode)`
- `callback`: `void (*)()`
- `mode`: `Rising`, `Falling`, or `Change`

`bool read()`
- Returns current comparator output level (`true` = high)

`void invert(enable)`
- Inverts comparator output polarity when `enable` is `true`

`bool pwm(outPin, freqHz)`
- Starts hardware PWM on `outPin` (must be a timer pin) and enables OCREF clear (OCREF_CLR_INT)
- `freqHz` is in Hz (you can use `250_kHz` / `1_MHz` literals)

`bool pwmClear(outPin, enable=true)`
- Enables/disables OCREF clear (pulse cut) on an *existing* PWM timer channel.
- This does **not** reconfigure PWM frequency/duty.
- Useful if you start PWM with `comp1.pwm(...)` but want `comp2` to also be able to gate/cut it.
- Returns `false` if `outPin` is not a timer pin or the timer doesn't support OCREF clear.

`bool pwmBreak(outPin, enable=true, activeHigh=true)`
- Enables/disables a *hardware* PWM cutoff using the timer **BREAK** input.
- Intended for fast protection (OV/OC): no ADC loop required.
- `activeHigh=true` means “break is active when comparator output is high”.
- Break is typically **level-based**: PWM stays cut while the comparator output remains active.
- The core enables automatic output restore (`AOE`) so PWM can come back automatically once the comparator output becomes inactive.
- When disabling (`enable=false`), the core also attempts to restore PWM outputs (clears break flag + re-enables `MOE`).
- Returns `false` if `outPin` is not on the supported timer or routing is not available on this MCU.

Notes:
- On **PY32F002A** and **PY32F003**, the device headers expose SYSCFG routing bits to connect **COMP1/COMP2** into **TIM1 BRK** on supported packages.
- On other PY32 parts, available routing targets (or availability) may differ.

`void duty(permille)`
- Sets PWM duty in 0..1000 (1000 = 100%)

**Notes / limitations**
- No extra "enable" is required in sketches. COMP is enabled by default in this core; it will only be unavailable if comparator support is disabled at build time.
- `attachInterrupt()` enables the shared `ADC_COMP_IRQn` NVIC interrupt (ADC/COMP share the IRQ on PY32F0xx).
- `pwm()` currently targets timers that support OCREF clear; on PY32F002A this is expected to be `TIM1`. If `pwm()` returns `false`, the selected `outPin` is not compatible.

**PY32F002A: `TIM1` pins that work with `comp1.pwm(outPin, ...)`**

`pwm()` picks the timer/channel automatically from the variant pin map.

Core-level (variant pin map) `outPin` values that resolve to `TIM1`:

`PA0, PA1, PA3, PA7, PA8, PA9, PA10, PA11, PA13, PB0, PB1, PB3, PB5, PB6`

Notes:
- Whether a pin is *physically available* depends on the exact **package/board** (some symbols may exist in the core, but the pad is not bonded out on smaller packages).
- `PA13` is commonly used for **SWDIO** (debug). If you are using SWD, avoid using `PA13` as PWM output.
- Complementary outputs (`CHxN`) can appear on some pins; on small packages these pins may not be bonded out.

If you want the "definitely works on *my board*" answer: test it. `pwm()` returns `false` if the pin is not compatible.

```cpp
Comparator comp1(1);
const uint32_t candidates[] = {PA0, PA1, PA3, PA7, PA8, PA9, PA10, PA11, PA13, PB0, PB1, PB3, PB5, PB6};

void setup() {
   comp1.begin(PA1, VREF_1_2, NC);
   comp1.duty(500);
}

void loop() {
   for (uint32_t p : candidates) {
      bool ok = comp1.pwm(p, 1000);
      delay(500);
      // if ok==true, you should see ~1kHz PWM on that pin.
   }
}
```

**PY32F003: `TIM1` pins that work with `comp1.pwm(outPin, ...)`**

`pwm()` picks the timer/channel automatically from the variant pin map.

Notes:
- Whether a pin is *physically available* depends on the exact **package/board**.
- If you want the “definitely works on my board” answer: try it; `pwm()` returns `false` if the pin is not compatible.
- `outPin` is currently a **software mirror** of the comparator output (updated on `read()` and on comparator interrupts). Use `NC` if you don’t need it.

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
