# Distributed-Gesture-IoT
Browser-Based-Gesture-Control
# EdgeVision-ESP32: AI Hand-Controlled LEDs 🖐️💡

This project allows you to control 5 physical LEDs using real-time hand gestures. It uses a smartphone as an "AI Brain" to process vision data and sends commands to an ESP32 microcontroller over WiFi.

I started this project in Class 11 and successfully completed it in Class 12 after many failed attempts. It proves that you don't need expensive hardware like a Raspberry Pi to do advanced AI-Robotics!

## 🚀 How it Works
1. **The Vision**: A smartphone camera captures your hand movements.
2. **The AI**: A browser-based model (MediaPipe) runs on the phone to count your fingers.
3. **The Bridge**: Data is sent via a local WiFi connection to the ESP32.
4. **The Action**: The ESP32 switches 5 LEDs based on the finger count (0-5).



## 🛠️ Hardware Requirements
* **ESP32** Development Board
* **5 LEDs** (Any color)
* **Smartphone** (Tested specifically on **Microsoft Edge**)
* **WiFi Connection**

## 🔌 Circuit Diagram
Connect the long legs (Anode) of the LEDs to these GPIO pins:
* LED 1: **GPIO 13**
* LED 2: **GPIO 12**
* LED 3: **GPIO 14**
* LED 4: **GPIO 27**
* LED 5: **GPIO 26**
* All short legs (Cathode) go to **GND**.



## 💻 How to Use
1. **Flash the Code**: Upload the `.ino` sketch to your ESP32.
2. **Serial Monitor**: Open the Serial Monitor to find the IP Address (e.g., `192.168.1.XX`).
3. **Open Browser**: Open **Microsoft Edge** on your Laptop and type in that IP Address.
4. **Test First**: Use the "Simple Toggle" test to make sure your LEDs work.
5. **Start AI**: Tap the **"START AI CAMERA"** button and show your hand to the camera!



## ⚠️ Known Issues & Troubleshooting
* **Browser Security**: Browsers block the camera unless you manually click "Start." If it hangs, refresh the page.
* **Compatibility**: This project was built and tested specifically on **Microsoft Edge**. Other browsers might have issues with camera permissions.
* **Lighting**: If the LEDs flicker, ensure your room is well-lit so the AI can track your hand landmarks.
* **Startup Bug**: If the ESP32 doesn't boot, unplug the LED on Pin 12, press Reset, and plug it back in (Pin 12 is a strapping pin).

## ❤️ Special Thanks
* **My Parents**: For giving me the space and support to keep trying through all the failures.
* **My Friends**: For the help and the distractions that kept me sane!
* **Gemini AI**: For acting as a digital co-engineer and helping me debug the toughest parts of this project.

---
**Author:** Vishwajeet Satish Lohar
**Grade:** Class 12 (2026)
**Status:** Successfully Working!
