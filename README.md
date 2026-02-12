# ⌚ Smartwatch 3D Simulation — OpenGL Project

A fully interactive **3D smartwatch simulation** built in **C++** using **OpenGL 3.3 Core Profile**.

This project represents a full 3D expansion of the original 2D smartwatch simulation.  
The smartwatch screen functionality remains identical (Time, BPM, Battery), but is now rendered inside a complete first-person 3D environment.

Developed as part of the **Computer Graphics** course at the  
**Faculty of Technical Sciences, University of Novi Sad (FTN)**.

---

## 🚀 About the Project

The smartwatch interface is rendered on a 3D object attached to a hand model in a perspective scene.

The environment includes:

- Infinite running ground simulation  
- Repeating 3D building models  
- Dynamic lighting  
- Camera movement  
- Focus mode interaction  

The application runs with a fixed **75 FPS** frame limiter.

---

## ✨ Features

### 🕒 Time Screen
- Real-time digital clock (`HH:MM:SS`)
- Automatic time rollover
- FreeType text rendering
- Screen navigation via clickable arrows

---

### ❤️ BPM Screen
- Animated EKG waveform
- Resting BPM between **60–80**
- Hold **D** to simulate running
- BPM increases gradually while running
- Camera bobbing effect
- Infinite ground movement
- Critical alert overlay when BPM exceeds 200

---

### 🔋 Battery Screen
- Battery drains **1% every 10 seconds**
- Color-coded battery levels:
  - 🟩 Green (>20%)
  - 🟨 Yellow (20–10%)
  - 🟥 Red (≤10%)
- Battery at 0% closes the application
- Percentage rendered using FreeType

---

## 🎥 Camera & Interaction

- Perspective projection
- Mouse controls vertical camera rotation
- Press **SPACE** to enter Focus Mode:
  - Watch moves in front of camera
  - Camera movement locks
  - Cursor becomes visible
  - Screens can be switched with mouse clicks
- Press **SPACE** again to return

---

## 🌍 Scene System

- Textured infinite ground segments (auto-reset behind camera)
- Repeating 3D building models
- Global light source (directional)
- Smartwatch screen acts as weak emissive light
- Hand and watch move together (viewmodel-style attachment)

---

## 🎨 Rendering Pipeline

- OpenGL 3.3 Core Profile
- Custom vertex & fragment shaders
- Depth testing
- Face culling
- Alpha blending
- Framebuffer Object (FBO) for watch screen
- Assimp for 3D model loading
- FreeType for text rendering
- stb_image for textures
- GLM for transformations

---

## 🛠️ Built With

- C++17  
- OpenGL 3.3 Core  
- GLFW  
- GLEW  
- GLM  
- Assimp  
- FreeType  
- stb_image  
- Visual Studio 2022  

---

## 🎮 Controls

| Action | Input |
|--------|--------|
| Move camera (up/down) | Mouse |
| Enter / Exit focus mode | SPACE |
| Switch screens | Left click (focus mode) |
| Simulate running | Hold **D** (BPM screen only) |
| Exit application | ESC |

---

## 👤 Author

**Katarina Zgonjanin**  
Faculty of Technical Sciences  
University of Novi Sad  
Computer Graphics — 2026

