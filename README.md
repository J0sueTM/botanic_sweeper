# Botanic Sweeper

A monstruosity of a game that combines MineSweeper with gardening.

## Building

After cloning the repo

```bash
git clone --recursive https://github.com/J0sueTM/botanic_sweeper
```

Create the needed folders:

```
mkdir bin
mkdir out
mkdir lib
```

Compile raylib and copy it to lib

```bash
cd vendor/raylib/src
make PLATFORM=PLATFORM_DESKTOP
cd ../../..
cp vendor/raylib/src/libraylib.a ./lib
```

Then run make

```bash
make
```

## Running

The executable will be in `./bin/botanic_sweeper`, you can run it with:

```bash
./bin/botanic_sweeper
```

A project by [Josué Teodoro Moreira](https://j0suetm.com).
