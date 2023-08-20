# Prettier-PDF
This is an attempt at making a small native app which does the job.

# Motivation
Scanned documents need to be enhanced before printing, otherwise their legibility is just poor.
Limited internet makes native apps more reliable.

# Progress
- [x] Filters
- [x] Import PDFs / Images
- [x] Export PDFs / Images
- [ ] Bulk edit
- [ ] Create a installer
- [ ] Upload as a package

# Dependencies
- Xpdf CLI tools
- Qt 5
- OpenCV 4

# How to build
Setup Visual Studio, use CMake.

## Windows
Install vcpkg. Use it to install qt & opencv.

## Linux
Both opencv and qt can be installed from package managers. Look for *-dev packages.

# How to distribute
For older Windows you need to package additional DLLs along with the executable.

# Inspiration
Other open source projects, tutorials.
