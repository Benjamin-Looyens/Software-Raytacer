# Software Raytracer
Software raytracer implemented with SDL2 for the course "Graphics Programming" in Howest DAE Game Development. The project implements sphere and plane primitives alongside triangle meshes. These objects use PBR materials (particularly, `Lambert`, `LambertPhong` and `CookTorrence`).

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