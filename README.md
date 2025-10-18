### Not a 'real' project; this is just an experiment

**NOTES:** 
- Can be compiled for Linux, Windows, and Web 
- Only tested on x86_64 Linux (including through wine)
- Linux version compiles for X11 (but I have only tested though xwayland)
- Defaults to linking Linux Raylib so won't compile on other platforms

run:
```
cc -o nob nob.c
./nob
```

compile options:
```
# windows
./nob -win

# web
./nob -web

# run
./nob -r

# together
./nob -win -r
./nob -r -web
```


non native platform requirements:
- for web: python3, [emcscripten](https://emscripten.org/docs/getting_started/downloads.html)
- for windows cross-compilation: x86_64-w64-mingw32, wine (optional for running)
