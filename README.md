# Interactive 3D Virtual Museum

A navigable 3D museum built in **C++ with OpenGL and GLSL**, combining real-time graphics, interactive exhibits, animation and environmental education.

Visitors explore three thematic rooms — marine life, endangered species and a planetarium — where proximity-based interactions reveal educational content, animations can be triggered, and a HUD tracks the user's progress through the museum.

Developed as the final project for the *Computer Graphics and Human-Computer Interaction* course at UNAM, Faculty of Engineering.

⬇️ [**Download the installer**](https://github.com/KevinSantiag0/Interactive-3D-Museum/releases/latest) (Windows)  
📄 [**Technical Report**](docs/TechnicalReport.pdf)  
📘 [**User Manual**](docs/UserManual.pdf)

---

## My Role

Academic team project developed by a **team of 5 students**. My work focused on programming and integration, project organization, partial 3D modeling and documentation.

- **Interaction and exhibit logic** — co-implemented the proximity-based interaction system used to detect nearby exhibits and trigger contextual actions such as animal information panels, educational panel zoom and planetarium activation.
- **HUD and room progression** — co-implemented the on-screen interface and the state system that tracks each room as *unvisited → in progress → completed* and displays a completion message after all three areas have been explored.
- **Integration and interactive presentation** — integrated interactive elements into the OpenGL scene, including environmental audio, models, textures and user-facing controls.

---

## Technical Highlights

### Interaction system

Interactions are implemented using **distance-based trigger volumes**.

Each interactive exhibit stores a world-space position and an interaction radius. At runtime, the application computes the Euclidean distance between the interaction point and each exhibit using `glm::distance()`.

The interaction point depends on the current camera mode:

- in free-camera mode, the camera position is used;
- in first- and third-person modes, the player position is used.

This approach kept interaction logic simple and inexpensive for a scene of this scale while allowing the same system to drive animal information panels, educational posters, room detection and planetarium controls.

Its main limitation is that triggers are spherical and based only on distance: they do not perform ray casting, visibility tests or object-level collision detection.

### Animation

Several animation techniques coexist because the museum contains objects with different motion requirements.

**Skeletal/keyframe animation** is used for imported animated characters and fish.
**Procedural motion** is used when behavior must be generated continuously at runtime.
**Transformation-based animation** is used for deterministic motions such as planetary orbits, rotations and interactive scene objects.

The fish system combines both approaches: each fish runs its imported skeletal animation while its world-space position is updated procedurally. Fish move with individual speed and direction values, reflect their direction vector when reaching the boundary of the pool, and use a sinusoidal vertical offset to produce a swimming motion.

The planetarium uses runtime transformations to calculate orbital positions around the Sun and allows the user to enable or disable planetary motion interactively.

### Lighting, materials and environment

The scene uses a multi-light **Phong illumination model** with configurable ambient, diffuse and specular material properties.

Different material configurations are used for scene elements. Water, for example, uses strong specular reflection and alpha blending to produce a translucent surface, while a separate material configuration is used for the animated astronaut.

A six-face **cube map** surrounds the museum and provides the environment background.

The rendering pipeline also includes dedicated shaders for procedural animation, skeletal animation, water effects, text rendering and the 2D interface.

### Camera system

Three camera modes can be switched at runtime:

- **Free camera** — independent navigation through the scene.
- **First-person camera** — positioned at the player's eye level and aligned with the player's viewing direction.
- **Third-person camera** — positioned behind and above the player while looking toward the character.

Mouse input updates yaw and pitch, while movement in character-controlled modes is constrained to the horizontal plane.

### HUD and room progression

The museum maintains a state for each thematic room.

Entering a room marks it as visited, while the HUD visually communicates whether a room is currently being explored or has already been completed. Once all three rooms have been visited and the player has left the active room, a temporary museum-completion message is displayed.

The same 2D rendering layer is used for contextual prompts, animal information panels, the controls overlay and interaction hints.

### Environmental audio

Environmental audio is managed using **irrKlang**.

The application detects the room currently occupied by the visitor and switches the looping ambient track accordingly. Separate audio environments are used for the aquarium, the planetarium, the endangered-species room and the central museum area.

A track is only replaced when the detected room changes, avoiding unnecessary audio restarts.

---

## Technology Stack

**Graphics & programming** — C++ · OpenGL 3.3 · GLSL · GLFW · GLAD · GLM

**Assets & rendering** — Assimp · stb_image · FreeType · Blender

**Audio** — irrKlang

**Development** — Visual Studio · MSVC

---

## Running It

Download the installer from the [latest release](https://github.com/KevinSantiag0/Interactive-3D-Museum/releases/latest), run it, and launch `MuseoVirt` from the install directory. Models, textures, shaders and audio are bundled.

Source code is under `Code/`. The project builds through a Visual Studio solution targeting Windows.

---

## Limitations

- **Windows-only.** The project targets a Visual Studio solution and ships as a Windows installer, so it does not build or run on macOS or Linux without reworking the build configuration. Migrating to CMake would remove this constraint.
- **Distance-only interaction triggers.** Exhibit detection relies on spherical volumes without ray casting or visibility tests, so an exhibit can be triggered through a wall if the visitor is close enough on the other side.

<!-- TODO: add 1-2 more from the Limitations section of the technical report.
     Candidates worth checking: absence of spatial partitioning (render cost scales
     linearly with scene objects), shadow handling, collision precision, model load
     times or memory footprint, hardcoded scene layout. Write only what was true. -->

---

## Controls

| Action | Key | Action | Key |
|---|---|---|---|
| Move | W / A / S / D | Controls menu | TAB |
| Look | Mouse | Wireframe | M |
| Camera modes | F1 / F2 / F3 | Normal rendering | N |
| Animal information | F | Point rendering | B |
| Panel zoom | V | Return to start | O |
| Planetary system | P | Exit | ESC |

The full control list is in the user manual and in-app via **TAB**.

---

## Team & Context

UNAM, Faculty of Engineering — Computer Engineering.
Course: Computer Graphics and Human-Computer Interaction. Completed May 2026.

Team of 5 students, including:

- Josue Cardoso Martínez
- Kevin Santiago González

<!-- TODO: list the remaining team members, or remove this line if you prefer to keep
     only the two names and state the team size above. -->

## Acknowledgements

Built with OpenGL, GLFW, GLAD, GLM, Assimp, stb_image, FreeType and irrKlang. Third-party libraries retain their own licenses. Full references are listed in the technical report.

## License

MIT — see [LICENSE](LICENSE).
