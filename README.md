Boxology
========

An editor to prototype box-like architectures (with cognitive robotics in
mind!).

![A dummy architecture](doc/example_editor01.png)

The UI has been build on top of
[qt5-node-editor](https://github.com/rochus/qt5-node-editor) by Nicolai Waniek.
Thanks to him!

Main features
-------------

- Convenient UI to create nodes for each processes;
- Connect them together;
- Label them with their main cognitive role;
- Save/load in a simple JSON format;
- Export to PNG, SVG and LaTeX (TikZ).

Requirements
------------

- Qt5
- Boost (successfully tested with 1.58, but older version may work equally well)
- `qmake` or `CMake >= 3.0`

Supported platforms
-------------------

[![Windows Build status](https://ci.appveyor.com/api/projects/status/bfwtu7orjfwxookx?svg=true)](https://ci.appveyor.com/project/severin-lemaignan/boxology)

- Linux (main dev platform)
- Windows (compiles and runs fine with QtCreator + VS2015). Download the
  [lastest binary for Windows 64bit](https://ci.appveyor.com/project/severin-lemaignan/boxology/build/artifacts).
