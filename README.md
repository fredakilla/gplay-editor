# gplay-editor
gplay-editor is a game editor for [**GPlayEngine**](https://github.com/fredakilla/GPlayEngine)

It extends the internal in-game editor of GPlayengine, based on Imgui, with an additionnal GUI using Qt to enjoy advanced widgets and other Qt features.

A node editor is integrated and can easily be extended to create any kind of content based on a nodal generation (particles, material, shaders, texture, etc ...)

Currently a particle generator for the spark particle engine is fully integrated and can be used to create and export particles effects for games using the SPARK particle library.


 
## Screenshots
<img src="https://i.imgur.com/tfIqiPa.png" width="50%" height="%">

## Current status
gplay-editor is in developpement and is not yet fully functional.  
Development is done on linux.   
Building on another system is not currently supported however final release should be cross-platform.


## Features
- C++ and Qt based interface.
- gplay-editor internally use [**GPlayEngine**](https://github.com/fredakilla/GPlayEngine) for rendering particles and bind the SPARK particle engine.
- node core system is provided by the external qt [**nodeeditor**](https://github.com/paceholder/nodeeditor) library.

TODO
- [x] implement genric nodes classes
- [x] automatic node's parameters widgets creation from simple declaration
- [x] show 3D debug zones geometries
- [x] serialize node widget's parameters
- [ ] ~~add +,- buttons to add/remove dynamic inputs on some nodes using lists (emitters, groups)~~
- [x] graph plot editor widget for spark graph interpolators (WIP but maybe do this using imgui)
- [ ] complete all spark nodes (95% done)
- [x] interact with 3D view (camera move...) + use the in-game editor
- [ ] multiple scenes managment
- [ ] possibility to attach result effect to scene nodes (static node, node in motion, trow bullet, ...)
- [ ] ~~add pliable parameters button to show/hide parameters widgets~~
- [x] comments items on node view
- [ ] set styles
- [ ] add shortcuts on node view : copy/paste nodes, repeat last created node,...

## Build

gplay-editor developpement is done with Qt 5.11.1 + QtCharts module for tgraph plot editor widget.   

WARNING : Do not use Qt 5.11.2, I experienced some bugs with FileDialogs on linux...


##### 1. Place the gplay-editor directory at same top level of gplay-engine :
(build gplay-engine using GP_CUSTOM_PLATFORM define)
```bash
├── YourDirectory
│	├── GPlayEngine
│	├── gplay-editor
```

##### 2. Get submodules
```
git submodule init
git submodule update
```

##### 3. Build the external nodeeditor Qt library
```
cd 3rdparty/nodeeditor
mkdir BUILD
cd BUILD
make
```

##### 4. Build editor
```
- Open gplay-editor.pro with QtCreator.  
- Build and Run.
```