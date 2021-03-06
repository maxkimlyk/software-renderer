# software-renderer

This project is a challenge of writing minimal possible real time 3D-renderer using only
the graphical functionality like "paint pixel with a certain color" provided by WinAPI or X11.
Developing of such project was inspired by reading [this acticles](https://github.com/ssloy/tinyrenderer/wiki).

## Building
In Linux:

You need _CMake_, _make_ and _libx11-dev_ installed.
```bash
software-renderer$ mkdir build && cd build
software-renderer/build$ cmake -DCMAKE_BUILD_TYPE=Release ..
software-renderer/build$ make
```

In Windows:

You need _CMake_ and _Visual Studio_.
```bash
software-renderer$ mkdir build && cd build
software-renderer/build$ cmake ..
software-renderer/build$ cmake --build . --config Release
```

**Note**: building in "release" mode makes code working much faster (about 10 times more FPS).

## Running
Some examples read resource files from currect directory, thus they should be run only from "bin" directory. For example, to run "cube":
```bash
software-renderer/build$ cd bin
software-renderer/build/bin$ ./cube
```
