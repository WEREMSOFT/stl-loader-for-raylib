# spine-raylib

<img align="left" src="./assets/raylib_logo.png" width="256">

The stl-loader-for-raylib runtime provides functionality to load, manipulate and render stl files on raylib.

One of the most amazing features of [Raylib](https://www.raylib.com/), besides the ease of use and its numerous ports to several languages, is the ability to compile to [WebAssembly](https://webassembly.org/), allowing to show your games around the world with just a link.

## Known Bugs
None AFAIK 

## Licensing
You are welcome to evaluate the Spine Runtimes and the examples we provide in this repository free of charge.

You can integrate the Spine Runtimes into your software free of charge, but users of your software must have their own [Spine license](https://esotericsoftware.com/spine-purchase). Please make your users aware of this requirement! This option is often chosen by those making development tools, such as an SDK, game toolkit, or software library.

In order to distribute your software containing the Spine Runtimes to others that don't have a Spine license, you need a [Spine license](https://esotericsoftware.com/spine-purchase) at the time of integration. Then you can distribute your software containing the Spine Runtimes however you like, provided others don't modify it or use it to create new software. If others want to do that, they'll need their own Spine license.

For the official legal terms governing the Spine Runtimes, please read the [Spine Runtimes License Agreement](http://esotericsoftware.com/spine-runtimes-license) and Section 2 of the [Spine Editor License Agreement](http://esotericsoftware.com/spine-editor-license#s2).

## Examples

This CMake-based project is an example of how to use the library on a 3D setting.

### Example
[Example](https://htmlpreview.github.io/?https://github.com/WEREMSOFT/stl-loader-for-raylib/blob/master/html/main.html)

<img src="./doc_images/example.png" width="300">

## Setup

1. Use the file src/main.c as a starting point.
1. Compile and run. If you need extra help compiling in the different platforms, check the awesome [Raylib documentation](https://github.com/raysan5/raylib/wiki) under the **Development Platforms** section.

## Notes

The function and defines is on stl_loader.h.

This project is based on CMake as a build pipeline.
