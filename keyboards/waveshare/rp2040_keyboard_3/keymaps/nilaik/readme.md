# nilaik keymap for waveshare/rp2040_keyboard_3

A 3-key macropad keymap with single-key layer cycling and a binary layer indicator.

## Behavior

- **Key 0 (first key)** is the layer control on every layer:
  - **Tap** -> advance to the next layer (`0 -> 1 -> ... -> 7 -> 0`).
  - **Hold** (>= 200 ms) -> jump straight back to layer 0.
- **Keys 1 and 2** default to copy / paste; edit them per layer in `keymap.c`.

## Layer indicator (RGB)

The 3 RGB LEDs show the current layer number in **binary** (LED 0 = bit 0 / LSB,
LED 1 = bit 1, LED 2 = bit 2). A lit LED is a set bit, so layer 0 is all off and
layer 7 is all on. This makes all 8 layers distinguishable.

## Build

- `qmk compile -kb waveshare/rp2040_keyboard_3 -km nilaik`
- `make waveshare/rp2040_keyboard_3:nilaik`

## Notes

Edit `keymap.c` to change each key in `LAYOUT(...)`.
