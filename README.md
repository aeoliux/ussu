# runas
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

Not on every system `persist` will work. You can disable it by running command bellow instead of `cmake ..`:
```sh
cmake -DDISABLE_PERSIST=ON ..
```

## Configuration
Default configuration will be installed to `${CMAKE_INSTALL_PREFIX}/etc/runas.conf` (by default `/usr/local/etc/runas.conf`).
Default configuration should always work for your user, if your user is in `wheel` group.

## Usage
`runas [ -U uid ] [ -G gid] [ -S ] [ command ]`:
- `-U` -> set custom user id (default is 0),
- `-G` -> set custom group id (default is 0),
- `-S` -> run shell<br>
`-S` or command is required.
