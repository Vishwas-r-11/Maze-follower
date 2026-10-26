# Autonomous Obstacle-Avoiding Robot with Turn Counter & LCD

An Arduino-based autonomous wheeled robot that navigates environments by detecting obstacles using an ultrasonic sensor mounted on a scanning servo. It logs navigation statistics—specifically left and right turn counts—and displays them on an I2C 16x2 LCD display controlled via a cycle push-button.

---

## 📋 Features

- **Autonomous Navigation**: Continuously scans forward and navigates around obstacles automatically.
- **Servo-Assisted Ultrasonic Scanning**: Sweeps left and right to determine the path with the most clearance when an obstacle is within 6 cm.
- **Turn Telemetry**: Tracks cumulative left and right turns executed during navigation.
- **I2C 16x2 LCD Display**: Outputs turn statistics clearly without cluttering digital I/O pins.
- **Push-Button State Machine**: Single button interface for toggling states:
  - **Press 1**: Start autonomous obstacle avoidance mode.
  - **Press 2**: Stop all motors.
  - **Press 3**: Display right-turn count on the LCD.
  - **Press 4**: Display left-turn count on the LCD and reset mode cycle.

---

## 🛠️ Hardware Requirements

| Component | Quantity | Notes / Typical Model |
| :--- | :--- | :--- |
| **Microcontroller** | 1 | Arduino Uno / Nano |
| **Motor Driver** | 1 | L298N, L293D, or similar H-Bridge driver |
| **DC Motors + Wheels** | 2 | Geared TT DC motors (Left & Right) |
| **Ultrasonic Sensor** | 1 | HC-SR04 |
| **Micro Servo** | 1 | SG90 9g Micro Servo (carries ultrasonic sensor) |
| **LCD Display** | 1 | 16x2 LCD with I2C Backpack (`0x27`) |
| **Push Button** | 1 | Momentary tactile switch |
| **Power Supply** | 1–2 | 7.4V–12V battery pack for motors and Arduino |
| **Chassis** | 1 | 2WD robot chassis with caster wheel |
| **Jumper Wires & Breadboard**| - | As needed |

---

## 📌 Pin Configuration

### DC Motor Driver (H-Bridge)
| Arduino Pin | Label in Code | Description |
| :--- | :--- | :--- |
| **D3** | `MLa` | Left Motor Direction / PWM Speed |
| **D9** | `MLb` | Left Motor Direction |
| **D6** | `MRa` | Right Motor Direction / PWM Speed |
| **D5** | `MRb` | Right Motor Direction |

### Ultrasonic Sensor (HC-SR04)
| Arduino Pin | Label in Code | Connection |
| :--- | :--- | :--- |
| **D11** | `trigpin` | Trigger Pin |
| **D10** | `echopin` | Echo Pin |
| **5V** | - | VCC |
| **GND** | - | GND |

### Servo Motor & Push Button
| Arduino Pin | Label in Code | Description |
| :--- | :--- | :--- |
| **D7** | `servo_pin` | Servo Signal Line |
| **D2** | `buttonPin` | Tactile Button (Configured with `INPUT_PULLUP`) |

### I2C LCD Display (16x2)
| Arduino Pin | LCD Pin | Description |
| :--- | :--- | :--- |
| **A4 (SDA)** | SDA | Serial Data |
| **A5 (SCL)** | SCL | Serial Clock |
| **5V** | VCC | Display Power |
| **GND** | GND | Ground |

---

## 📦 Required Libraries

Install the following libraries via the **Arduino IDE Library Manager** (`Sketch` > `Include Library` > `Manage Libraries...`):

1. **[NewPing](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home)** by Tim Eckel (Optimized ultrasonic sensor library)
2. **Servo** (Built-in standard library)
3. **Wire** (Built-in standard library for I2C)
4. **[LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)** by Frank de Brabander

---

## ⚙️ How It Works

```
         [ Start / Default (Press 1) ]
                       │
             Obstacle <= 6 cm?
              ├── No  ──> Move Forward (PWM speed compensation)
              └── Yes ──> Stop -> Back up slightly -> Stop
                                │
                      Look Left & Look Right
                                │
         ┌──────────────────────┴──────────────────────┐
   Left Clearance > 20 cm                       Right Clearance > 20 cm
         │                                             │
    Turn Left                                     Turn Right
  lcount = lcount + 1                           rcount = rcount + 1
```

1. **Button Cycles**:
   - **State 1**: Executes `moveforward()`. If an obstacle is detected within 6 cm, the vehicle pauses, rolls back briefly, and positions the servo to sweep right ($10^\circ$) and left ($180^\circ$). It prioritizes turning toward whichever direction has clearance $> 20\text{ cm}$.
   - **State 2**: Invokes `movestop()` to brake all motors.
   - **State 3**: Clears LCD and prints the total number of right turns (`rcount`).
   - **State 4**: Clears LCD, prints the total number of left turns (`lcount`), and resets the counter to 0.
2. **Motor Balance**: The code applies differential PWM values (`MRa = 72`, `MLa = 60`) to compensate for motor drift and keep the robot tracking straight.

---

## 🚀 Getting Started

1. **Wire the Components**: Connect the motors, sensors, servo, and LCD according to the pin configuration table above.
2. **Verify I2C Address**: Confirm your LCD backpack address is `0x27`. If your module uses `0x3F`, update the following line in the sketch:
   ```cpp
   LiquidCrystal_I2C lcd(0x3F, 16, 2);
   ```
3. **Open the Sketch**: Load your `.ino` file into the Arduino IDE.
4. **Select Board & Port**: Go to `Tools > Board` and pick your Arduino board, then choose the appropriate serial port under `Tools > Port`.
5. **Upload**: Hit the **Upload** button.
6. **Operate**: Press the tactile button on Pin 2 to start autonomous navigation. Press again to halt or view turn counts.

---

## 🔧 Troubleshooting & Calibration

- **Robot Doesn't Drive Straight**: Adjust the PWM values inside `moveforward()` (`analogWrite(MRa, ...)` and `analogWrite(MLa, ...)`) until the chassis drives in a straight line.
- **Servo Center Offset**: Adjust `servo_pin.write(115)` and `servo_pin.write(110)` in your sketch to match your servo horn's mechanical center.
- **Button Debounce Issues**: If the button skips states, inspect the pull-up wiring on pin 2 or increase `delay(300)` in the button polling block.
