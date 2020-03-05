# CSci-4611 Assignment 3:  Earthquake

This program visualizes data provided by NASA of every earthquake around the world from years 1910-2000. It utilizes the MinGfx graphics library which works off the OpenGL API. I create a textured mesh of the Earth and draw spheres where each earthquake occurs. You can switch the view of the Earth between a 3D globe and 2D map.

* Earth mesh:  
  The earth is defined as a 360x180 mesh (one vertex for each degree). I initialize it by iterating by pi/180 from [-pi/2, pi/2] in the y and [-pi, pi] in the x. It properly maps the Earth texture to the mesh.

* Earthquakes:
  * Use QuickShapes::DrawSphere to draw all earthquakes which have occured in the past year by iterating from the quake which occured last year to the current time's earthquakes. 
  * I map the earthquakes to the mesh by converting the latitude/longitude to the corresponding 3D coordinates. Due to the way my mesh is defined (one vertex for each degree), I simply need to draw the earthquake at it's associated lat/long (in Radians) using the sphere formulas given in the project write-up. As I start off with a globe, the position of each quake is defined as the linear interpolation of the globe coordinates to the plane coordinates.
  * The earthquakes vary in size and color based off of their magnitude:
    * Moderate: ML < 6.0 = Small, Blue (only show up ~1965)
    * Strong - Great: 6.0 < ML < 8.0 = Medium, Orange
    * Great: ML > 8.0 = Large, Red

  * These are scaled both off their actual magnitude in relation to the largest from the dataset (9.7ML), and some constant value.
* Globe/Plane:
  * The mesh starts off as a globe and can be morphed between a globe and plane when the "Globe" button is pressed. The earthquakes are interpolated along with the Earth.
  
* Extra features:
  * When you drag the mouse it rotates the earth! (and resets it when the Globe button is pressed).
