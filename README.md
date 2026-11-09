# 3D Custom OpenGL C++ Engine (Met Bonus-Features)

Een data-driven 3D rendering engine geschreven in C++ en OpenGL (Core Profile 3.3). De engine ondersteunt het inladen van 3D-scenes via externe configuratiebestanden (`scene.txt`), real-time camera- en multi-belichtingsbesturing, Blinn-Phong shading met meerdere lichtbronnen, SGI-materiaalpresets en textuurmapping.

---

## Features & Behaalde Uitbreidingen

* **Data-driven Scene Management**: Wereld-objecten, hun posities, schaling, opaciteit, materialen en texturen worden ingeladen vanuit een extern `scene.txt` bestand.
* **Shape Factory Pattern (Inclusief Kegel/CONE)**: Ondersteuning voor 5 primitieven (`CUBE`, `PYRAMID`, `CYLINDER`, `SPHERE` en de extra bonus-vorm `CONE`).
* **Meerdere Lichtbronnen (Multi-Lighting)**: De fragment shader verwerkt een array van 2 actieve lichtbronnen cumulatief (een beweegbaar hoofdlicht en een paars/blauw sfeerlicht).
* **Blinn-Phong Shading & SGI Materialen**: Realistische belichting met ondersteuning voor optische SGI-materiaalpresets (zoals `GOLD`, `RUBY`, `EMERALD`, `CYAN_PLASTIC`) én de mogelijkheid voor fototexturen zonder kleurverstoring.
* **FPS Camera System & Zoom**: Vrije 3D-camerabeweging met WASD-toetsen, muiskijkrichting en dynamische Zoom (FOV) via het scrollwiel.
* **Visual Debuggers**: Fysiek zichtbare bol-markers voor beide lichtbronnen in de 3D-ruimte.

---

## Technische Stack

* **Taal**: C++17
* **Graphics API**: OpenGL 3.3 (Core Profile)
* **Windowing & Input**: GLFW 3
* **Extension Loader**: GLEW
* **Wiskunde**: Eigen `Matrix4`/`Transform` klasse gecombineerd met GLM (`glm::mat4`, `glm::perspective`)
* **Textuur Loader**: `stb_image.h`

---

## Compileren en Uitvoeren

### Compilatiecommando (MSYS2 UCRT64 / MinGW)
```bash
g++ main.cpp engine.cpp shapes.cpp -o app.exe -lglew32 -lglfw3 -lopengl32 -lgdi32
./app.exe
```

---


## Besturing

| Toets / Invoer | Actie |
| :--- | :--- |
| **W / A / S / D** | Camera naar voren, links, achteren en rechts bewegen |
| **Spatie / Left Shift** | Camera omhoog / omlaag bewegen |
| **Muis** | Rondkijken (Pitch & Yaw met beveiliging tegen Gimbal Lock) |
| **Scrollwiel** | In- en uitzoomen (dynamische FOV aanpassing) |
| **I / K** | Hoofdlichtbron (Light 1) naar voren / achteren verplaatsen |
| **J / L** | Hoofdlichtbron (Light 1) naar links / rechts verplaatsen |
| **U / O** | Hoofdlichtbron (Light 1) omhoog / omlaag verplaatsen |
| **C** | Lichtkleur van Light 1 wisselen (7 varianten: Wit, Warm Zonlicht, Rood, Groen, Blauw, Cyan, Magenta) |
| **R** | Rotatie van de 3D-objecten pauzeren / hervatten |
| **1 of 2** | veranderen van lightbron |
| **ESC** | Het venster en de engine afsluiten |