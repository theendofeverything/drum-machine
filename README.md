Crummy keyboard drum machine for me to play along to whatever I'm
listening to while I'm working.

# Build

```
$ make
g++ -std=c++20 -Wall -Wextra -Wpedantic -Iinc `pkgconf --cflags sdl2` `pkgconf --cflags SDL2_mixer` -o build/main src/main.cpp `pkgconf --libs sdl2` `pkgconf --libs SDL2 _mixer`
Run: ./build/main
Specify window x y w h:
Run: ./build/main 680 15 680 316
```

# Run

```
$ ./build/main 
./build/main
```

# Play

- `Space` - kick
- `i` - snare
- `j` - snare
- `Shift` - hold shift to make snare louder

# TODO

- [ ] master volume control
- [ ] more sounds
- [ ] user assigns sounds to keys
- [ ] save user settings
