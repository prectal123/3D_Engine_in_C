# 🧊 Minimal CPU 3D Renderer (C++)

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
- Applies matrix transformations using **Eigen**
- Projects 3D coordinates into 2D screen space
- Renders output using **OpenCV**
- Runs entirely on a **single-threaded CPU pipeline**

It is designed as an educational and experimental 3D rendering core that can be extended or adapted for embedded or hardware-constrained environments.

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

---

## 🧠 Design Philosophy

This project intentionally avoids GPU acceleration to:

- Deeply understand rendering mathematics
- Control pixel-level behavior manually
- Maintain portability for experimental or embedded systems

Because it is pure C++, the core logic may be adaptable to:

- Arduino-compatible C++ environments (with heavy constraints)
- Embedded systems with framebuffer access
- Custom hardware pipelines

> ⚠ Note: Actual portability depends on C++ standard support, available memory, and hardware constraints.

---

## 🧩 Current Limitations

- Single-threaded CPU rendering
- No GPU acceleration
- Limited shading
- No full texture mapping
- Performance not optimized for large meshes

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
