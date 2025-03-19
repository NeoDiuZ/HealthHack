# 🧠 Neural Drive: Neural Control System

**Neural Drive is the next generation of Brain-Computer Interfaces that translates your inner voice into commands to control the world around you.**

![Neural Drive Logo](https://i.imgur.com/x8fWOST.png)

## 🌟 Overview

Neural Drive is a revolutionary brain-computer interface (BCI) system that enables control of physical devices through thoughts alone. Unlike invasive alternatives, our solution uses affordable surface electrodes to capture and process subvocal signals, making neural control technology accessible to everyone.

Our system empowers users to control assistive technologies for mobility and home automation with nothing more than their thoughts—transforming accessibility and independence for millions.

## 🏆 Why Neural Drive Excels

Healthcare is shifting from reactive treatment to proactive, AI-driven prevention. Neural Drive tackles this transformation by:

- **💰 Radical Affordability**: Neural Drive is 100x cheaper than competing solutions, making neural control accessible to underserved populations and healthcare systems with limited resources.

- **🌐 Broader Use Cases**: Neural Drive extends beyond mobility to support mental health monitoring, rehabilitation exercises, medication adherence tracking, and even cognitive training applications.

- **🏠 At-Home Healthcare**: Our non-invasive approach eliminates the need for clinical settings, reducing healthcare costs and enabling continuous care in patients' homes.

- **💪 Empowering Independence**: Our technology transforms healthcare accessibility by enabling users to control devices without physical interaction.

### 🥇 Competitive Advantage

Unlike our competitors, Neural Drive offers:

| Feature | Neural Drive | Neuralink | Emotiv |
|---------|----------|-----------|--------|
| **Invasiveness** | ✅ Non-invasive | ❌ Requires surgery | ✅ Non-invasive |
| **Cost** | ✅ Under $100 | ❌ Thousands of dollars | ❌ $500-$1000+ |
| **Setup Time** | ✅ Seconds | ❌ Surgical procedure | ✅ Minutes |
| **Training Time** | ✅ Under 30 seconds | ❌ Weeks of calibration | ❌ Hours of training |
| **Accessibility** | ✅ Universal | ❌ Medical approval required | ✅ Consumer available |

## 🧩 System Components

### 🖥️ Web Interface (`page.tsx`)

A React-based user interface that provides:

- Feature management (add/remove control features)
- Recording calibration data for each feature
- Training the classification model
- Real-time prediction monitoring
- Visual feedback on system status

![Web Interface Screenshot](https://i.imgur.com/x8fWOST.png)

### ⚙️ Backend Server (`app.py`)

A Flask and WebSocket server that:

- Captures subvocal signals via serial connection
- Processes signals using Fast ICA
- Trains classification models using PyCaret
- Provides REST API endpoints for feature management
- Broadcasts binary control signals via WebSocket

![Backend Server Screenshot](https://i.imgur.com/fsvEEEH.png)

### 🏠 IoT Device: House Controller (`2ch_house.ino`)

ESP32-based device that:

- Connects to WiFi and WebSocket server
- Controls a continuous rotation servo (fan simulation)
- Drives a NeoPixel 8x8 LED matrix for visual feedback
- Responds to binary commands (0/1):
  - `1`: Activates servo rotation and rapid color cycling
  - `0`: Stops servo and turns off LEDs

[House Controller Video]

https://github.com/user-attachments/assets/c57695b2-08ba-4894-88ee-bfae0551482e

### 🚗 IoT Device: Car Controller (`2ch_car.ino`)

ESP32-based device that:

- Connects to WiFi and WebSocket server
- Controls two DC motors for vehicle movement
- Drives a NeoPixel 8x8 LED matrix for visual indicators
- Responds to binary commands (0/1):
  - `1`: Activates forward motion with animated green arrows
  - `0`: Stops motors and displays a red stop sign

[Car Controller Video]

https://github.com/user-attachments/assets/b61d0a1e-b615-4533-8232-0086ec7bd34f

## 🔧 Setup Instructions

### Prerequisites

- **Python 3.7+**: Powers the backend server and machine learning components
- **Node.js and npm**: Required for the React frontend application
- **ESP32 microcontrollers**: Act as the brain of the IoT devices, receiving commands from the server
- **Subvocal signal amplifiers**: Low-cost ($10) EMG sensors that capture neural signals
- **Arduino IDE with ESP32 support**: Used to program the IoT devices

### 🐍 Backend Setup

1. Install Python dependencies:
   ```bash
   pip install flask flask-cors pandas numpy scikit-learn pycaret serial websockets
2. Connect your subvocal signal amplifier to your computer via USB
3.Run the Flask server: python app.py


### 🌐 Frontend Setup
1.Install Node.js dependencies: npm install
2. Start the development server: npm run dev
3. Access the web interface at http://localhost:3000

### 🔌 IoT Device Setup
1. Open the Arduino IDE and install the following libraries:
   -**WiFi: Enables internet connectivity**
   -**ArduinoWebsockets: Establishes communication with the server**
   -**ESP32Servo: Controls servo motors**
   -**Adafruit_NeoPixel: Manages LED displays**

2. For each device (house and car):
   -**Open the corresponding .ino file**
   -**Update WiFi credentials (SSID and PASSWORD)**
   -**Update WebSocket server details (WS_HOST and WS_PORT)**
   -**Verify and upload the code to the ESP32 device**
   -**Monitor the serial output to confirm successful connections**


   ## 📋 User Workflow

### Step 1: Physical Setup

- Attach the adhesive electrode patches to the appropriate facial/neck muscle focal points.
- Connect the electrodes to the signal amplifier.
- Connect the amplifier to your computer via USB.

### Step 2: Calibration and Training

- **Add Features**: Using the web interface, choose up to 8 words/commands you want to train (e.g., "Yes", "No", "On", "Off").
- **Record Calibration Data**:
  - For each word, click its button to start a 15-second recording session.
  - Subvocalize (think about saying without actually speaking) the word repeatedly.
  - The system automatically captures and processes your neural patterns.

- **Train Model**: Once all features are calibrated, click "Train Model" to generate your personalized neural classifier (takes under 30 seconds).

### Step 3: Control and Use

- **Start Processing**: Click "Start Real-time Processing" to begin classification.
- **Control Devices**:
  - Think about your trained words to activate corresponding functions.
  - The system will continuously classify your subvocal signals.
  - When detected, commands are wirelessly transmitted to IoT devices.

## 🛠️ Technology Stack

Neural Drive leverages a diverse set of modern technologies to create a seamless brain-computer interface experience:

### 🌐 Frontend
- **⚛️ React.js**
- **⏭️ Next.js**
- **🎨 Tailwind CSS**
- **🔍 Lucide-React**
- **🧩 Shadcn/UI**

### 🔙 Backend
- **🐍 Python**
- **🌶️ Flask**
- **📡 WebSockets**
- **🤖 PyCaret**
- **🧠 scikit-learn**
- **🔢 NumPy/Pandas**
- **📊 FastICA**

### 💻 Hardware & IoT
- **⚡ ESP32**
- **🔌 Arduino Framework**
- **💡 NeoPixel LEDs**
- **📈 EMG Sensors**
- **🔄 Serial Communication**

### 🔄 Data Processing Pipeline
1. **📥 Signal Acquisition**
2. **🧹 Signal Preprocessing**
3. **🔍 Feature Extraction**
4. **🏷️ Classification**
5. **📤 Command Transmission**

This integrated stack enables Neural Drive to process neural signals with minimal latency (under 100ms) while maintaining high classification accuracy and reliable device control.

## Common Use Cases

- **Mobility Assistance**: Control wheelchairs, prosthetics, or other mobility devices.
- **Home Automation**: Manage lights, fans, doors, and appliances.
- **Communication Aids**: Enable speech for those who cannot physically speak.
- **Computer Control**: Navigate interfaces without physical interaction.

## 🔍 Troubleshooting

- **Connection issues**: Ensure the correct USB port is being used in `app.py` (see `get_port()` function).
- **Training failures**: Verify that sufficient data was recorded for each feature.
- **ESP32 connectivity**: Check WiFi credentials and WebSocket server IP/port configuration.
- **Serial port conflicts**: Try running `cleanup_port()` manually or restart your system.
- **Signal quality**: Ensure electrodes have good skin contact and the amplifier is properly connected.

## Conclusion: Unlocking Your Inner Professor X !!!!!

Neural Drive represents a significant leap forward in brain-computer interface technology, essentially giving users Professor X-like abilities to control their environment with thoughts alone. Like the iconic X-Men character, Neural Drive users can influence objects and devices without physical contact—just pure mental command.

By making neural control accessible, affordable, and non-invasive, we're democratizing a technology that was previously limited to research labs, high-budget medical facilities, or the pages of comic books. Our solution transforms ordinary people into extraordinary telepathic commanders of their environment.

With just $10 worth of sensors, some open-source software, and affordable IoT components, we've created a system that can translate thoughts into actions. This isn't about the future—it's about now. Today.

The implications for healthcare, accessibility, and human-computer interaction are profound:

- The wheelchair user becomes someone who controls their mobility with thoughts alone.
- The person with limited motor function becomes capable of managing their smart home environment independently.
- The individual with speech impairments gains a new way to communicate without physical effort.

Neural Drive isn't just a technology demonstration—it's a glimpse into a future where the gap between thought and action is seamlessly bridged through intelligent, accessible interfaces. The boundaries between human intention and technological action are dissolving.

Join us in this revolution to make neural control technology available to everyone who could benefit from it, and experience the Professor X within you.
