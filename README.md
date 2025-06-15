# GameSense
GameSense is an AI-powered dynamic commentary generation system designed to bring real-time, insightful, and engaging commentary to games, sports, and other interactive experiences. Leveraging Generative AI, GameSense analyzes gameplay or events and crafts high-quality, adaptive narratives, enhancing audience engagement.

Project built as part of the Qualcomm EDGE AI Hackathon, Bangalore.

Team Members :
Hemang Mohan 
Vivek Kumar Yadav
Archana M
Ponathipan Jawahar

Socials:
|                 |                                                                  |
|---------------------|-------------------------------------------------------------------------|
| Hemang Mohan        | [LinkedIn](https://www.linkedin.com/in/hemang-mohan-7a7a9415a/) |
| Vivek Kumar Yadav   | [LinkedIn](https://www.linkedin.com/in/vivenkumarxr/) |
| Archana M           | [LinkedIn](https://www.linkedin.com/in/archana-madhavan-8381461ab/) |
| Ponathipan Jawahar  | [LinkedIn](https://www.linkedin.com/in/ponathipan-jawahar-33a91481/) |

GameSense runs on Snapdragon® NPU that delivers live, AI-powered cricket commentary using a large language model (LLM, e.g., [Llama 3.2 3B](https://aihub.qualcomm.com/compute/models/llama_v3_2_3b_instruct)) accelerated by the Genie SDK.

---

## Requirements

### Platform

- Snapdragon® Platform (e.g., X Elite)
- Windows 11+

### Tools and SDK

- Visual Studio 2022  
  - Download from [Visual Studio](https://visualstudio.microsoft.com/vs/)
  - Ensure "Desktop development with C++" is selected during installation
- QAIRT SDK: [Qualcomm AI Runtime SDK](https://qpm.qualcomm.com/#/main/tools/details/Qualcomm_AI_Runtime_SDK)  
  - See [QNN SDK](https://qpm.qualcomm.com/#/main/tools/details/qualcomm_ai_engine_direct) for older versions

### Python Dependencies

For the Flask middleware, install the required Python packages:

```bash
pip install -r ChatApp/requirements.txt
```

---

## Build Instructions

### 1. Generate Genie Bundle via AI Hub

1. Clone this repository to obtain a local copy of `GameSense`.

2. Follow the [LLM on Genie tutorial](https://github.com/quic/ai-hub-apps/tree/main/tutorials/llm_on_genie) to generate the `genie_bundle` required by GameSense.  
   - If you use a Llama 3 model, GameSense will work out of the box.
   - For other models, update the prompt format in `PromptHandler.cpp` as needed.

3. Copy the generated bundle assets to `GameSense\genie_bundle`.  
   - You should see files like `GameSense\genie_bundle\*.bin` and `genie_config.json`.

### 2. Setup QAIRT SDK

Ensure the QAIRT (or QNN) SDK version installed matches the version used by AI Hub to generate your context binaries.  
You can find the required version in the AI Hub compile job details.

1. Download and install the [Qualcomm AI Runtime SDK](https://qpm.qualcomm.com/#/main/tools/details/Qualcomm_AI_Runtime_SDK).
2. Set the global environment variable `QNN_SDK_ROOT` to the root path of your QAIRT SDK installation, e.g.: "C:\Qualcomm\AIStack\QAIRT\2.32.0.250228"

3. Verify your installation by running:
```bash
ls ${env:QNN_SDK_ROOT}/lib
```
If this command lists the SDK libraries, your setup is correct.

---

## Build Project in Visual Studio 2022

1. Ensure `QNN_SDK_ROOT` is set globally before building.
2. Open `GameSense.sln` in Visual Studio.
3. Build the project.

---

## Running GameSense

### Via Visual Studio

- Click the green play button to build and run.
- The project is configured with these command-line arguments:

```bash
.\ARM64\Debug\GameSense.exe --genie-config .\genie_bundle\genie_config.json --base-dir .\genie_bundle\
```

### Via Command Line

```bash
cd {Project directory}
.\ARM64\Debug\GameSense.exe --genie-config .\genie_bundle\genie_config.json --base-dir .\genie_bundle\
```

- Run with `--help` to see all options:

```bash
.\ARM64\Debug\GameSense.exe --help
```

#### File Path Notes

- Use `\\` or `/` as path separators, not a single `\`.

**Correct:**
C:\Path\To\Model\Config\llama2_7b.json C:/Path/To/Model/Config/llama2_7b.json

**Incorrect:**
C:\Path\To\Model\Config\llama2_7b.json

#### Unicode Support

For languages requiring Unicode, see:  
[UTF-8 support instructions](https://github.com/quic/ai-hub-apps/blob/main/tutorials/llm_on_genie/powershell/README.md#utf-8-support)

---

## Flask Middleware Setup

The Flask middleware handles audio streaming and commentary generation. It accepts game objects from the game, sends them to the backend, and converts the generated commentary to audio.

### Setup Instructions

1. Navigate to the `ChatApp` directory.

2. Install the required Python packages:

```bash
pip install -r requirements.txt
```

3. Run the Flask middleware:

```bash
python SocketStream.py
```

The middleware will start a server on `http://0.0.0.0:5000`.

### API Usage

- Send POST requests to `/get_audio` with a JSON body containing the game object.
- The server streams audio commentary in WAV format.

Example:

```bash
curl -X POST http://127.0.0.1:5000/get_audio -H "Content-Type: application/json" -d '{"game_event": "Player scored a goal!"}'
```

---

## Real World End-to-End Use Case

### VR Cricket Game

The VR Cricket Game is a Unity-based virtual reality application tailored for devices like the Meta Quest. It leverages advanced physics engines and VR controller integration to deliver a realistic cricket batting simulation. Key features include:

- **Physics-Based Ball Movement**: Simulates real-world cricket ball trajectories, including spin and bounce, using Unity's physics engine.
- **Controller Grip Mechanics**: Enables players to interact with the bat dynamically, providing precise control over shots.
- **Dynamic Scoring System**: Calculates runs based on ball trajectory, timing, and shot accuracy, ensuring an authentic gameplay experience.
- **6-Ball Over Format**: Implements a structured gameplay mode with a live scoreboard for tracking player performance.

### Backend Integration

The game integrates with a Flask-based backend to:
- Evaluate player scores using AI-driven analytics.
- Generate real-time commentary powered by GameSense's LLM capabilities.
- Enhance immersion by delivering context-aware narratives during gameplay.

This project exemplifies the seamless integration of VR technology with AI-powered commentary systems, creating an engaging and immersive experience for cricket enthusiasts.

Demo Video: [Watch Demo](https://drive.google.com/file/d/1P50k12IsAczm4QMJP1O5tnUryYXqizRO/view?usp=sharing)

---