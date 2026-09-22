/*
 * ============================================================
 *  Electrosummer 2026 - Laboratory No. 1
 *  Electrochemistry Unplugged: Make Your Own Potentiostat
 *  Repository: electrosummer-potentiostat
 *  Schematic:  ../schematic/schematic.png (see also netlist.txt)
 *  Build guide: ../docs/build_instructions.md
 * ============================================================
 *
 * Minimal Educational Potentiostat Code for Seeeduino XIAO (SAMD21)
 * 
 * Hardware Pin Mapping:
 *   VIN  (U2.1 / A0) -> Control Input (DAC)
 *   VREF (U2.2 / A1) -> Reference Electrode Voltage (ADC)
 *   VOUT (U2.3 / A2) -> TIA Output Voltage / Current (ADC)
 * 
 * Data Output Columns:
 *   1. Time (ms)               - Elapsed time since scan started
 *   2. Set Potential (V)       - Commanded voltage from DAC (V_DAC - V_VGND)
 *   3. Measured Cell Voltage (V)- Actual measured WE - RE voltage
 *   4. Current (uA)             - Measured cell current via TIA
 */

const int pinDAC  = A0; // VIN
const int pinVref = A1; // VREF
const int pinVout = A2; // VOUT

// Constants
const float V_REF_MCU      = 3.3;      // XIAO analog reference voltage
const float VIRTUAL_GROUND = 1.65;     // VGND voltage (10k/10k divider from 3.3V)
const float TIA_RESISTOR   = 1450.0;  // R3 in Ohms

// Cyclic Voltammetry Sweep Settings
const int dacCenter = 512;  // 1.65V (0V vs VGND)
const int dacMin    = 202;  // ~0.65V (~ -1.0V vs VGND)
const int dacMax    = 822;  // ~2.65V (~ +1.0V vs VGND)
const int stepDelay = 15;   // Time between steps in ms

void setup() {
  Serial.begin(115200);

  // SAMD21 Hardware Resolutions
  analogWriteResolution(10); // 10-bit DAC (0 to 1023)
  analogReadResolution(12);  // 12-bit ADC (0 to 4095)

  // Hold cell at equilibrium (0V vs VGND)
  analogWrite(pinDAC, dacCenter);

  while (!Serial); // Wait for Serial Monitor connection
  Serial.println("Ready. Press 'Enter' in the Serial Monitor to start CV scan.");
}

void loop() {
  // Trigger scan on Enter key ('\n' or '\r')
  if (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      runCyclicVoltammetry();
    }
  }
}

void runCyclicVoltammetry() {
  Serial.println("\n--- Starting Cyclic Voltammetry Scan ---");
  Serial.println("Time_ms,Set_Voltage_V,Measured_Cell_Voltage_V,Current_uA");

  // Record scan start timestamp
  unsigned long startTime = millis();

  // Phase 1: Ramp up (0V -> +1.0V)
  for (int dac = dacCenter; dac <= dacMax; dac++) {
    measureStep(dac, startTime);
  }

  // Phase 2: Ramp down (+1.0V -> -1.0V)
  for (int dac = dacMax; dac >= dacMin; dac--) {
    measureStep(dac, startTime);
  }

  // Phase 3: Return to center (-1.0V -> 0V)
  for (int dac = dacMin; dac <= dacCenter; dac++) {
    measureStep(dac, startTime);
  }

  // Return cell to safe rest potential (0V vs VGND)
  analogWrite(pinDAC, dacCenter);
  Serial.println("--- Scan Complete. Press Enter to run again. ---");
}

void measureStep(int dacVal, unsigned long scanStartTime) {
  // 1. Apply potential
  analogWrite(pinDAC, dacVal);
  delay(stepDelay); // Settling time

  // 2. Timestamp (ms from start of scan)
  unsigned long elapsedTime = millis() - scanStartTime;

  // 3. Calculate Commanded / Set Potential vs Virtual Ground
  float vDac = (dacVal / 1023.0) * V_REF_MCU;
  float setVoltage = vDac - VIRTUAL_GROUND;

  // 4. Sample ADC channels
  int rawVref = analogRead(pinVref);
  int rawVout = analogRead(pinVout);

  // 5. Convert raw ADC to actual voltages
  float vRef = (rawVref / 4095.0) * V_REF_MCU;
  float vOut = (rawVout / 4095.0) * V_REF_MCU;

  // 6. Calculate Measured Cell Potential (V_cell = WE - RE)
  float cellVoltage = VIRTUAL_GROUND - vRef;

  // 7. Calculate Cell Current (I = (VGND - VOUT) / R_TIA)
  float current_uA = ((VIRTUAL_GROUND - vOut) / TIA_RESISTOR) * 1e6;

  // 8. Output CSV line
  Serial.print(elapsedTime);
  Serial.print(",");
  Serial.print(setVoltage, 3);
  Serial.print(",");
  Serial.print(cellVoltage, 3);
  Serial.print(",");
  Serial.println(current_uA, 2);
}