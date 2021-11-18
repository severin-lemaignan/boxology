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
- Export to ROS (see below for details)

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
  
  
ROS support
-----------

ROS nodes can be automatically generated from the Boxology architecture.

All Boxology nodes are converted into simple ROS nodes in a ROS workspace (ie, all nodes are generated under a `src/` subdirectory, and they can all be built with eg `catkin build` from the top directory).

Boxology attempts to convert the nodes' inputs and outputs into topic subscribers and publishers. This work best if the inputs/ouputs follow the syntax ``/name/of/topic [datatype/MyDataType]`` (for instance: ``/camera/color/image_raw [sensor_msgs/Image]``. In that case, Boxology will insert the correct headers, and also create the package dependencies in `package.xml` and `CMakeLists.txt` (if topic name and datatype are not specified, default publishers/subscribers using `std_msgs/Empty` messages are created).

Nodes whose name is `TF` or `tf` are not converted into ROS nodes. This makes it easy to indicate connections between nodes and the TF system by creating 'ghost' TF nodes where necessary. However, you can indicate that a node listens or broadcasts specific TF frames by adding ``tf: /frame`` inputs or outputs to your node.

The ROS nodes are generated from templates that can be found [here](templates/ros). If you want to modify these templates or create your own templates, the following fields are available:
```json
{
  "name": "<full name of architecture>",
  "id": "<id of the architecture, generated from name>",
  "version": "<version of the architecture>",
  "description": "<description of the architecture>",
  "path": "<export path>",
  "boxology_version": "<version of Boxology used to generate>",
  "nodes": [
      {
      "id": "<node id>",
      "id_capitalized": "<NodeId>",
      "label": "<node label (linked to the node's color)>",
      "name": "<full name of the node>",
      "version": "<version of the node>",
      "description": "<description of the node>",
      "boxology_version": "<version of Boxology used to generate>",
      "dependencies": [
          ["ROS package name", "ROS msg"],
          ["eg std_msgs", "String"],
          ...
       ],
       "packages":  ["ROS package name", "..."],
       "inputs": [
          {"name": "<raw input name>",
           "type": "topic|tf|undefined",
           "topic": "<topic> if type is 'topic' or 'undefined'",
           "frame": "<tf frame> if type is 'tf'",
           "short": "<short topic>, eg 'image_raw' instead of '/camera/color/image_raw'",
           "datatype": ["ROS package name", "ROS msg"]
          },
          ...
        ],
        "outputs": [
          {"name": "<raw output name>",
           "type": "topic|tf|undefined",
           "topic": "<topic> if type is 'topic' or 'undefined'",
           "frame": "<tf frame> if type is 'tf'",
           "short": "<short topic>, eg 'image_raw' instead of '/camera/color/image_raw'",
           "datatype": ["ROS package name", "ROS msg"]
          },
          ...
        ]
      },
      ...
  ]
}
```
