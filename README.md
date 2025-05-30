# Line Following Fire Extinguisher Robot

![Line Follower Robot](assets/Line%20Follower.PNG)

This project is an Arduino-based robot that follows a black line using 5 digital line sensors and can detect and extinguish fire using a flame sensor and a fan. The robot uses PID control for smooth line following and an L298N motor driver for movement.

## Features
- **5 Digital Line Sensors**: For accurate line following (leftmost, left, center, right, rightmost)
- **PID Control**: Adjustable Kp, Ki, Kd for optimal line tracking
- **Flame Sensor**: Detects fire in front of the robot
- **Fan/Extinguisher**: Automatically activates when fire is detected
- **L298N Motor Driver**: Controls two DC motors for movement

## Pin Configuration
| Component         | Arduino Pin |
|-------------------|-------------|
| Flame Sensor      | 11          |
| Fan (Extinguisher)| A0          |
| Line Sensor LM    | 2           |
| Line Sensor L     | 3           |
| Line Sensor C     | 4           |
| Line Sensor R     | 5           |
| Line Sensor RM    | 6           |
| Motor ENA         | 9           |
| Motor IN1         | 7           |
| Motor IN2         | 8           |
| Motor ENB         | 10          |
| Motor IN3         | 11          |
| Motor IN4         | 12          |

> **Note:** Adjust pin numbers in the code if your wiring is different.

## How It Works
1. **Line Following**: The robot reads all 5 sensors and calculates the error for PID control. It adjusts the speed of left and right motors to stay on the line.
2. **Fire Detection**: If the flame sensor detects fire, the robot stops and activates the fan until the fire is gone.
3. **Resumes Line Following**: After extinguishing, the robot continues following the line.

## Getting Started
1. Wire all sensors, motors, and fan according to the pin configuration above.
2. Upload the `line_follower.cpp` code to your Arduino board using PlatformIO or Arduino IDE.
3. Adjust PID constants (`Kp`, `Ki`, `Kd`) and motor speeds as needed for your robot and environment.

## Tuning Tips
- Start with the default PID values and adjust for smoother line following.
- Make sure all sensors are properly aligned and calibrated.
- Test the fire detection and fan activation in a safe environment.

## License
This project is for educational purposes. Modify and use as needed.
