
# powerprompt

## Compiling

Before running CMake, do:

```
conan install -s build_type=Debug ..
```

## Installing

### Cygwin

Install [Nerd Fonts](https://www.nerdfonts.com/) on your Windows host.  I chose Cousine.

Install by adding something similar to this to your shell startup script, (`.bashrc`for me):

```
PS1='$(/cygdrive/c/dev/powerprompt/cmake-build-debug/bin/powerprompt.exe)'
```

