# Game of Life (GoL)
by Ondřej Hůlka

## Binaries

In this folder/directory there are three executable files: `GoL-standalone.exe`, `GoL-static` and `GoL.AppImage`.

**`GoL-standalone.exe`**
- Microsoft Windows executable with SFML and all of its dependencies statically linked.
- It should run without requiring any additional libraries to be installed.
- The exact command used to compile this executable can be found in `tasks.json` inside the `.vscode` folder, under the comment `standalone-release`.

**`GoL-static`**
- Linux executable with only SFML statically linked.
- To run it, all other dependencies must be installed on the system, although most of them are usually provided by the distribution.
- If the binary fails to run, the error is most likely caused by a missing dependency.

**`GoL.AppImage`**
- AppImage file, where there are all necessary dependencies already inside.
- If the AppImage fails to run, the error is most likely cause by a missing `fuse` package or an old version of `glibc`.

## Compiling for Microsoft Windows

If you want to compile the project without downloading SFML yourself, dynamic libraries (`.dll` files) are provided in the `bin` folder.

For Windows to find these libraries, you must either:
- Add the `bin` folder to the `PATH` environment variable, **or**
- Copy all `.dll` files into the same directory as the executable.

SFML requires a **specific compiler version**, which must be downloaded from the official SFML website:  
<https://www.sfml-dev.org/download/sfml/3.0.2/>

To compile the project, run the following command **from this directory**:

```sh
Path_to_SFMLs_version_of_g++ -O3 -g -Iinclude src/*.cpp main.cpp -Llib -lsfml-graphics -lsfml-window -lsfml-system -o GoL.exe
```

Optionally, you can add the compiler’s directory to the beginning of the PATH environment variable and use g++ instead of the full path.

## Compiling for Linux

If you want to compile the project without installing SFML system-wide, dynamic libraries (`.so` files) are provided in the `lib` folder.
To compile the project, run the following command **from this directory**:

```sh
g++ -O3 -DNDEBUG -DSFML_STATIC src/*.cpp main.cpp -o GoL -Llib -Wl,-Bstatic -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -Wl,-Bdynamic -lX11 -lXrandr -lXcursor -lXi -lGL -lfreetype -lharfbuzz -ljpeg -lbz2 -lpng16 -lz -lbrotlidec -lbrotlicommon -ludev -lpthread -ldl -lm
```

## Configuration File

There is a `config.ini` file where you can change certain settings, mainly `grid_width` and `grid_height` without recompiling the whole project.
If the file does not exist, all configuration values are initialized with default values.
