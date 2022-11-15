# DDGDataViewer
Program and dependency-less library that let's you open and view Densha de go game files.
The library currently supports reading dat packs, images, 3d models, area's (maps) and some track information.

Works with Densha de go final! and games that use similar engine (e.g. profesional 2).

The standalone library is in the DDG folder and works with any C++11 (or better) compiler.

To compile the program you need qt5, cmake and a C++11 (or better) compiler.

Planned features:
- Editing and then saving textures
- Reading speed limit, station and railway sign data
- Reading station names if this data exists in the files
- Reading game audio data (shouldn't be difficult to figure out since VLC can open the games audio files already)
- Editing and saving 3d models
- Editing and saving area's

