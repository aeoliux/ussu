# ussu
A tool for authentication user accounts for POSIX-compatible operating systems.

## Building
### Dependencies
- Working C library

### Build dependencies
- A C compiler
- cmake

### Compilation and installation
```sh
mkdir -p build && cd build
cmake ..
make install
```

On some systems `persist` may not work. You can disable it by running command bellow instead of `cmake ..`:
```sh
cmake -DDISABLE_PERSIST=ON ..
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
