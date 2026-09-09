# Interactive 3D Virtual Museum

An interactive 3D virtual museum developed in **C++ using OpenGL and GLSL** as a final project for the *Computer Graphics and Human-Computer Interaction* course at the **National Autonomous University of Mexico (UNAM)**.

The application combines real-time rendering, 3D modeling, animation, shaders, user interaction, and educational content in a navigable virtual environment.

## Overview

The museum is divided into three thematic areas:

- **Marine Life**
- **Endangered and Extinct Species**
- **Planetarium**

Users can freely explore the museum, switch between camera modes, interact with exhibits, view educational information, activate animations, and track their progress through the different rooms.

## Technologies

- **C++**
- **OpenGL 3.3**
- **GLSL**
- **GLFW**
- **GLAD**
- **GLM**
- **Assimp**
- **stb_image**
- **FreeType**
- **irrKlang**
- **Blender**
- **Visual Studio 2022**

## Main Features

### Real-Time 3D Rendering
The application implements a complete real-time graphics pipeline using OpenGL and custom GLSL shaders.

### Lighting and Materials
The project includes:

- Phong lighting
- Ambient, diffuse, and specular components
- Fresnel-Schlick approximation
- Environment cube mapping
- Textured 3D models

### Animation Systems

Three animation approaches were implemented:

- Basic transformation-based animation
- Procedural animation
- Keyframe and skeletal animation

Examples include:

- Planetary rotation and orbit
- Moving stars
- Animated astronaut
- Animated fish
- Interactive doors and objects

### Camera System

The museum includes multiple camera modes:

- Free camera
- First-person camera
- Third-person camera

### Interactive Exhibits

The application detects the player's proximity to interactive objects.

Depending on the exhibit, users can:

- Display information about animals
- Zoom into educational panels
- Activate the planetary system
- Interact with animated objects

### Museum Progress System

A HUD tracks the user's progress through the three museum areas.

Rooms change state depending on whether they:

- Have not been visited
- Are currently being explored
- Have already been completed

After visiting all three rooms, the application displays a completion message.

### Audio

Environmental audio changes depending on the room currently being explored using the **irrKlang** audio library.

## Project Structure

```text
Interactive-3D-Museum/
│
├── Code/
│   └── 09_10_Animation/
│
├── Documentation/
│   ├── UserManual.pdf
│   └── TechnicalReport.pdf
│
└── README.md
```

## Running the Application

### Windows

1. Download `MuseoVirt.exe`.
2. Run the installer.
3. Follow the installation instructions.
4. Launch the application from the installed `MuseoVirt` directory.

The installer contains the resources required to run the museum, including models, textures, shaders, and other dependencies.

For detailed installation and control instructions, see:

`Documentation/UserManual.pdf`

## Controls

Some of the main controls include:

| Action | Key |
|---|---|
| Move | W / A / S / D |
| Move camera | Mouse |
| Camera modes | F1 / F2 / F3 |
| Animal information | F |
| Planetarium panel zoom | V |
| Activate planetary system | P |
| Controls menu | TAB |
| Wireframe mode | M |
| Normal rendering | N |
| Point rendering | B |
| Return to start | O |
| Exit | ESC |

More controls are documented in the user manual and can also be displayed inside the application by pressing **TAB**.

## Technical Documentation

The repository contains two supporting documents:

### Technical Report

`Documentation/TechnicalReport.pdf`

Includes:

- Project methodology
- Software architecture
- Graphics pipeline
- Modeling workflow
- Shader implementation
- Animation systems
- Interaction design
- Experiments
- Results
- Limitations and future improvements

### User Manual

`Documentation/UserManual.pdf`

Includes:

- Software requirements
- Installation instructions
- Controls
- Room descriptions
- Application usage

## My Contributions

This was a **team academic project**.

My work included participation in:

- Project conceptualization and design
- 3D modeling and optimization in Blender
- Model and texture integration into OpenGL
- C++ development and debugging
- Contextual proximity-based interactions
- On-screen menus and HUD elements
- Museum room progression system
- Environmental audio integration
- Testing and integration of graphics and interactive components

## Screenshots

Screenshots and a demonstration video will be added to showcase:

- Marine Life room
- Planetarium
- Endangered Species room
- Interactive information panels
- Camera modes
- Animation systems
- Museum progress HUD

## Future Improvements

Possible future improvements include:

- Web-based version of the museum
- Improved accessibility controls
- Additional museum rooms
- More interactive exhibits
- Educational assessment at the end of the experience
- Further graphics and performance optimization

## Academic Context

**National Autonomous University of Mexico — UNAM**  
Faculty of Engineering  
Computer Engineering  

Course: **Computer Graphics and Human-Computer Interaction**

Academic project completed in **May 2026**.

## Authors

- Josue Cardoso Martínez
- Kevin Santiago González

Additional collaborators participated in the development of the project.

## Acknowledgements

The project uses several external libraries and tools including OpenGL, GLFW, GLAD, GLM, Assimp, stb_image, Blender, FreeType and irrKlang.

Some external resources and tools were also used during the modeling and animation workflow. Full references and acknowledgements are available in the technical report.
