# ussu
A tool for authentication user accounts for POSIX-compatible operating systems.

## Building
### Dependencies
- C library
- POSIX-compatible libcrypt

### Build dependencies
- A C compiler
- cmake

### Compilation and installation
```sh
cmake .
make install
```

On some systems `persist` may not work. You can disable it by running command bellow instead of `cmake ..`:
```sh
rm -f CMakeCache.txt
cmake -DDISABLE_PERSIST=ON .
```

### Anti-brute force
If you want to prevent from being attacked by brute force method, add `-DANTI_BRUTE_FORCE=ON` to cmake options:
```sh
rm -f CMakeCache.txt
cmake -DANTI_BRUTE_FORCE=ON .
```

## Configuration
Default configuration will be installed to `${CMAKE_INSTALL_PREFIX}/etc/ussu.conf` (by default `/usr/local/etc/ussu.conf`).
Default configuration should always work for your user, if your user is in `wheel` group.

## Usage
```
ussu [ -U uid ] [ -G gid ] -C <or> -S <or> command
```
- `-U` -> set custom user id (default is 0),
- `-G` -> set custom group id (default is 0),
- `-S` -> run shell
- `-C` -> reset persist data. Only works if persist is enabled.
