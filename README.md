# 🧊 Minimal CPU 3D Renderer (C++)
A mathematically-derived CPU ray casting renderer implemented from first principles.

<p align="center">
  <img src="https://github.com/user-attachments/assets/d73be983-270d-4fcb-93d6-c1352e2d9a0a" width="300"/>
  <br/>
  <sub>
    model source :
    <a href="https://www.turbosquid.com/3d-models/apple-cartoon-3d-1495154">
      TurboSquid – Apple Cartoon 3D
    </a>
  </sub>
</p>
---

A lightweight 3D rendering engine built with **C++**, using **Eigen** for matrix computation and **OpenCV** for pixel-level visualization.

This project focuses on implementing the fundamental 3D graphics pipeline from scratch — without GPU APIs like OpenGL or DirectX.

---

## 📌 Overview

This renderer:

- Loads and parses `.obj` 3D model files
- Generates per-pixel viewing rays from a virtual camera
- Computes ray–triangle intersections
- Determines the closest hit per pixel
- Shades surfaces using geometric normal information

Unlike traditional rasterization pipelines, this engine does not project polygons directly onto screen space.  
Instead, it performs per-pixel ray casting from the camera into the scene.

This project originated from a personal mathematical study of 3D space, camera modeling, and geometric visibility.  
The rendering core was implemented directly from self-derived mathematical formulations rather than adapting an existing graphics API.


---

## 🛠 Tech Stack

- **Language:** C++
- **Linear Algebra:** Eigen
- **Visualization / Pixel Output:** OpenCV
- **Model Format:** Wavefront `.obj`

---

## 🚀 Features

### ✅ OBJ File Loader
- Parses vertex (`v`) and face (`f`) data
- Constructs mesh representation in 3D space

### ✅ 3D Transformation Pipeline
- Model, View, Projection matrix structure
- Homogeneous coordinate transformation
- Perspective projection

### ✅ CPU-based Rasterization
- Single-threaded rendering loop
- Manual pixel-level control via OpenCV buffer
- Basic depth handling

### ✅ Interactive Model Inspection
- Rotate / translate / inspect model in 3D space
- Real-time display window via OpenCV

### ✅ Ray Casting Rendering Core
- Per-pixel ray generation from camera origin
- Ray–triangle intersection testing
- Closest-hit depth resolution (no conventional Z-buffer raster stage)
- Surface shading based on triangle normals
---

## 🧠 Design Philosophy

This project began as a mathematical exploration of 3D space representation and camera-based visibility.

Before implementation, the rendering process was formally modeled using:

- Vector space geometry
- Parametric ray equations
- Ray–triangle intersection mathematics
- Projection derivations from first principles

The repository is being updated alongside documentation that explains the theoretical foundation and derivation process behind the engine.

The goal is not only to render images, but to build and document a rendering system that originates from mathematical theory.


> ⚠ Note: Actual portability depends on C++ standard support, available memory, and hardware constraints.

---

## 🧩 Current Limitations

- Brute-force triangle intersection (no BVH acceleration)
- Single-threaded CPU rendering
- Limited shading model
- No texture mapping
- Performance not optimized for large meshes

---

## 📘 Mathematical Documentation

This project is accompanied by a mathematical derivation document:

📄 **Ray Casting Renderer – Mathematical Derivation**  
[View Documentation](./Mathematical_Raycast_Renderer_Project_OverView.pdf)

The document includes:

- Camera coordinate formulation
- Ray construction from image plane
- Ray–triangle intersection derivation
- Closest-hit depth resolution model
- Geometric reasoning behind the rendering pipeline

The implementation was developed directly from these mathematical formulations.


---

## 🗺 Roadmap / ToDo

- [ ] Performance optimization
- [ ] Multi-core rendering option
- [ ] Sprite-based color mapping
- [ ] Texture mapping implementation
- [ ] Basic lighting model (Lambert / Phong)
- [ ] Z-buffer refinement
- [ ] Optional CUDA acceleration experiment

---
Dependencies
- Eigen
- OpenCV

Make sure Eigen and OpenCV are properly installed and discoverable by CMake.

---
📜 License

MIT License

Copyright (c) 2026 Prectal123@Github

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

---

🎯 Purpose

This engine exists as a learning-driven rendering experiment —
a foundation for exploring:

- Software rasterization
- Mathematical rendering pipelines
- Hardware-constrained 3D graphics
- Experimental engine design
