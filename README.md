# gplay-editor
gplay-editor is a scene editor for [**gplay 3D engine**](https://github.com/fredakilla/GPlayEngine)
 
## Screenshots
<img src="https://i.imgur.com/4uplO1O.png" width="50%" height="%">

## Current status
gplay-editor is in developpement and is not yet functional.

Development is done on linux, Building on another system is not currently supported however final release should be cross-platform.


## Build

gplay-editor developpement is done with Qt 5.11.1 + QtCharts module  
warning : Do not use Qt 5.11.2, I experienced some bugs with FileDialogs on linux...


##### 1. Place the gplay-editor directory at same top level of gplay-engine :
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