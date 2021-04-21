# Projects
Collection of relevant projects I developed pursuing my Bachelors of Science at the University of Minnesota.

## CAIPS_DesignDoc:
* The final deliverable for our group Software Engineering project. In this graduate class we attended an elicitation session with a client tasking us to design a system for a university class and instructor assignment system. Having only one interaction with this client, we wrote an official SRS document, designed the business layer of the system interfacing with the UI and database layers, and wrote test cases for each requirement.

## FireSim3D (Java - Processing):
* This is a fire simulation using Navier-Stokes continuous fluid dynamics based on a paper by Jos Stam - "Real-Time Fluid Dynamics for Games". In this paper, Stam details how to use the Navier-Stokes fluid equations to create a smoke simulation. After successfully replicating this technique, I extended it to making a fire-simulation which added additional complexities such as bouyancy forces to cause the flames to rise, dampening the particles so they don't fill the area, and attempting to extend it to a 3D environment requiring higher resolution.
* Project website with details of the features and a demonstration:  https://sites.google.com/d/1OZhpbYEqsyRRz-OgVV-HEp7BgbWvTkG7/p/10kkBrleZvmm-q3ZoC7ni2e7j62Ph5ytI/edit
* Final project for Animation and Planning graduate class. Idea and Research was done by myself with code referenced from Jos Stam's paper and modified to fit my project.

## MazePath (Java - Processing):
* This is my favorite work. It is a path-finding AI which solves any maze the User draws in the shortest distance. First, the User draws a closed-off maze with a start and an end point. Then the AI creates a probabilistic roadmap (PRM) based on random samples surrounding the walls of the maze and uses Uniform-Cost Search to solve the maze with the optimal path.
* Project website with details of the features and a demonstration:  
https://sites.google.com/view/mazesolver/home
* Created for Animation and Planning graduate class. All ideas, research, and code was done independently by myself.

## flocking_sim (Java - Processing):
* This program is a simulation of flocking behavior (such as birds, or in this case, bugs) and a rudimentary particle system. 
* Project website with details of the features and a demonstration:  
https://sites.google.com/view/flocking-sim/home
* Created for Animation and Planning graduate class. All ideas and code are my own but is based on examples from Dr. Stephen Guy.

## cloth_sim3D (Java - Processing):
* A 3D cloth simulation demonstrating how tension forces applied to a mesh of masses can simulate cloth-like behavior. 
* Project website with details of the features and a demonstration:  
https://sites.google.com/view/ac-cloth-sim/home
* Created for Animation and Planning graduate class. All code is my own but is based on the ideas and code of Dr. Stephen Guy.

## earthquakes (C++):
  * This program was an exercise in low-level graphics programming. It creates a detailed sphere mesh, maps a high-res bitmap image of Earth to the mesh, then interfaces with public data from NASA of every earthquake reading since 1910. It iterates through each year up to the 2000s and displays every earthquake that happened in the last year at it's corresponding location on the globe.
  * The code to interface to the NASA earthquake data was provided.
  
## Text Rain (Java - Processing):
  * An interactable graphics program written in the Java-based Processing environment. Letters fall from the top
  of a webcam like rain and you can lift them up/move them around with your body.
  
## Car Soccer (C++):
  * A small game based off Rocket League made as an introduction to the MinGfx graphics toolkit, utilizing the OpenGL API.
    Drive a car around and hit a soccer ball into a goal with realistic simulated physics.

## Operating Systems (Csci 4061) (Language: C)   
  These programs were written entirely by myself and partner, Azul Bolanos, with no framework given.     
  Each has a README with a description and summary of the work done.    
  - OS_mymake:  
    This program is an implementation of the Unix command 'make'.  
    Subjects: Process spawning (fork, exec), DFS stack implementation, input tokenizing.    
    
  - OS_MapReduce:  
    This program is an implementation of the Map-Reduce model of programming.  
    Subjects: Directory traversal, Inter-process communication, file redirection.  
  
  - ThreadedWordCount:    
    This program is a practice on multi-threaded synchronization and the Producer/Consumer model.   
    Subjects: Threads, Mutex Locks/Condition Variables, File I/O    
    
  - NetworkedMapReduce:\
    A different execution of the MapReduce program with intra-process communication over a TCP/IP network.\
    Subjects: Client/Server, TCP/IP networking, Multi-threading, process spawning, concurrency/synchronization.\
    
## Advanced Programming Principles (Csci 2041) (Language: Ocaml)
  - tautology.ml:  
    Tautology checker for propositional logic expressions  
    Subjects: High-level recursion, pattern matching    
      
  - PureLispScanner:    
    Scanner for a Pure Lisp Interpreter, it receives a Lisp expression and tokenizes it for a Parser to evaluate.   
    Contains source code (makeScanner) and a test file (exclaim.in)   
    Subjects: Higher-order functions, environments, closures    
