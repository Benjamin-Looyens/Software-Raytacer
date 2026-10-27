
# Software Raytracer
Software raytracer implemented with SDL2 for the course "Graphics Programming" in Howest DAE Game Development. The project implements sphere and plane primitives alongside triangle meshes. These objects use PBR materials (particularly, `Lambert`, `LambertPhong` and `CookTorrence`). The project runs entirely on the local CPU.

## Features
- Perspective camera:
    - WASD for 360 movement
    - RMB for camera shifting
    - LMB for diving (forward + camera)
    - RMB + LMB for camera panning
- Rendering
    - F2 toggles shadows
    - F3 cycles independent lighting modes
- Scenes
    - Z cycles scenes 
        - Material scene with 6 spheres and 3 triangles
        - Stanford bunny mesh scene
    - R toggles rotation for scenes
 
## Examples
<img width="642" height="514" alt="RaytraceMain" src="https://github.com/user-attachments/assets/539d0b91-6415-4118-9639-06a10c1dec18"/>
<img width="642" height="514" alt="RaytraceBunny" src="https://github.com/user-attachments/assets/b05a640e-4c46-4a5d-b858-9f0e80ba0bc6"/>
