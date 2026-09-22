# electrosummer-potentiostat

A minimal, four-op-amp, breadboard-buildable potentiostat built around a
single LM324 quad op-amp and a Seeeduino XIAO SAMD21 — made for
**Electrosummer 2026, Laboratory No. 1: "Electrochemistry Unplugged: Make
Your Own Potentiostat"**.

It implements the smallest set of building blocks a three-electrode
potentiostat needs:

- **VF** — a voltage follower, used twice: once to buffer a resistor-divider
  virtual ground, once to buffer the reference electrode (RE) without
  loading it.
- **CA** — the control amplifier, which drives the counter electrode (CE)
  to force the cell to the commanded potential.
- **TIA** — a transimpedance amplifier, which converts the working-electrode
  (WE) current into a voltage the microcontroller's ADC can read.

The firmware runs a cyclic-voltammetry (CV) sweep and streams
`time, set potential, measured cell voltage, current` as CSV over serial.

## Repository layout

```
firmware/potentiostat_CV.ino   Arduino sketch for the Seeeduino XIAO SAMD21
schematic/schematic.png        signal-flow schematic (didactic block diagram)
schematic/netlist.txt          exact pin-to-pin netlist, BOM and known issues
docs/build_instructions.md     detailed, step-by-step breadboard build guide
```

## Quick start

1. Read `schematic/schematic.png` and `schematic/netlist.txt`.
2. Follow `docs/build_instructions.md` to wire the circuit on a breadboard.
3. Upload `firmware/potentiostat_CV.ino` (Arduino IDE, board = "Seeeduino
   XIAO").
4. Open the Serial Monitor at 115200 baud, press **Enter** to start a scan.
5. Test first with a single 10 kΩ resistor between WE and CE (RE shorted to
   CE) — you should see a straight current-vs-voltage line through the
   origin. Then try a real cell, e.g. ≈5 mM K₃[Fe(CN)₆] in 0.1 M KCl.

## Safety

Wear nitrile gloves and safety glasses when handling the ferricyanide
(K₃[Fe(CN)₆]) solution or any other electrolyte. Dispose of chemical waste
according to your lab's protocol.

## Bill of materials

| Part | Value / part number | Qty |
|---|---|---|
| U1 | LM324N (quad op-amp, DIP-14) | 1 (+1 spare) |
| U2 | Seeeduino XIAO SAMD21 | 1 |
| R1, R2, R4, R5 | 10 kΩ | 4 |
| R3 | 1.45 kΩ (TIA gain resistor) | 1 |
| C1, C2 | 100 nF | 2 |
| C3 | 10 µF | 1 |

See `schematic/netlist.txt` for exact node-to-pin connections.

## License

Add your preferred open-hardware / open-source license here (e.g.
CERN-OHL-S for the hardware, MIT for the firmware) before publishing.
