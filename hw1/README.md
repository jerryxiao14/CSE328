# HW1

Jerry(Ruijie) Xiao

115297528

ruijie.xiao@stonybrook.edu

## Overview

- Implemented a sample modern OpenGL program with GLFW as the windowing toolkit. 
- Implemented a naive Bresenham line drawing routine without edge-case handling.
- Implemented edge case handing for Bresenham line drawing (negative slope, m>1)
- Implemented circle and Ellipse midpoint algorithms

## Notes

- All README files for future homework should also comply with the same format as this one. 
- This program template is just for your reference. Please feel free to code your own program (i.e., not using this template). However, the user interface (mouse and keyboard functionalities) should be the same as specified in the homework manual. 
- Please **comply with the submission requirements as detailed on the [TA Help Page](https://www3.cs.stonybrook.edu/~xihan1/courses/cse328/ta_help_page.html)**. Plesase rename the directory as instructed by the TA Help Page, and submit via Brightspace. 
- Please also make sure you have checked all implemented features with "x"s in the Markdown table below. As speficied on the TA Help Page, only checked features will be considered for grading!

## Dependencies

- OpenGL:
```bash
sudo add-apt-repository ppa:kisak/kisak-mesa
sudo apt update
sudo apt-get dist-upgrade
sudo reboot
```
- [GLAD](https://glad.dav1d.de/)
  - Configuration w.r.t. results of `sudo glxinfo | grep "OpenGL`
  - Command `glxinfo` needs `mesa-utils`
- Remaining dependencies could be installed via apt:
```bash
apt install libopencv-dev libglm-dev libglew-dev libglfw3-dev mesa-utils libx11-dev libxi-dev libxrandr-dev
```

## Compile & Run

- Run inside this directory, i.e., `hw1/`: 
```bash
mkdir build
cd build
cmake -DMAKE_BUILD_TYPE=Release ..
make 
cd ..
./build/hw1
```

## Features Implemented

Check all features implemented with "x" in "[ ]"s. 
Features or parts left unchecked here won't be graded! 

- [x] 1. Line Segment (Fully Implemented in This Template)
  - [x] 0 <= m <= 1
- [x] 2. Line Segment
  - [x] Slope m < -1
  - [x] -1 <= m < 0
  - [x] 1 < m
  - [x] Vertical
- [x] 3. Ploy-line & Polygon
  - [x] Poly-line
  - [x] Polygon
- [x] 4. Circle & Ellipse
  - [x] Circle
  - [ x Ellipse
- [ ] 5. Polynomial Curve (BONUS PART)
  - [ ] Line
  - [ ] Quadratic Curve
  - [ ] Cubic Curve
  - [ ] Super-Quadrics

## Usage

- If you have implemented extra functionalities not mentioned in the manual,
  you may specify them here.
- If your program failed to obey the required mouse/keyboard gestures,
  you may also specify your own setting here.
  In this case, penalties may apply.

## FAQ: Runtime error "shader file not successfully read"

If you are using the CLion IDE, you should set up the working directory of the project.
First click the "hw1 | Debug" icon on the top-right corner, 
next click "Edit Configurations...", 
then set up the "Working directory" item to the root of your project, 
i.e., the path to `hw1/`
(this directory should be further renamed to `sbuid_hwx` as specified above). 
Note that the working directory must be **exactly** root of your project 
(its parent directories, e.g. path to `StonyBrookCSE328Graphics/`, won't work). 

## Appendix

Please include any other stuff you would like to mention in this section.
E.g., format of your config file, and your suggestions on possible combinations of cubic curve parameters. 
