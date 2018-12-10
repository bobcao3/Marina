# Marina
A modern Wayland compositor that utilizes the GPU for more efficient desktop experience.

**This implementation is highly unstable and under very early development.**

# Building / Testing

Dependencies:
-   wlroots
-   egl / glesv2
-   wayland-server
-   wayland-protocols

For building the package, you also need:
-   wayland-scanner

To build Marina:
```sh
$ meson build
$ cd build
$ ninja
```

To test Marina:
```sh
$ cd build
$ ./marina-compositor
```