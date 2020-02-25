# Assignment 2: Car Soccer

## Features:
This program should implement all features from the C, B, and A ranges: 
* C Range:
    * Lines drawn around boundaries and 20x10 goals
    * Ball falls from the air initially with a random velocity (see: Ball.Reset()) and is given a new velocity whenever spacebar is pressed. I wasn't sure if you wanted the ball to reset from the starting position or not, so I just gave it a new velocity. If you did want it to reset that wouldn't be hard, can just call ball_.Reset() in the launch_ball function.
    * The ball experiences gravity when it is above ground and not colliding with car (line 176)
    * The ball collides with the ground and is reflected about the ground normal (y coordinate)
    
* B Range: 
    * Ball collides with walls/ceiling in the same fashion as the ground. (lines 73 - 135)
    * Car collides with walls in the same fashion
    * The ball reduces velocity due to friction at a rate of 0.85 for the walls/ceiling and 0.9 for ground

* A Range:
    * The ball collides with the car in the process described in the technical background (line 158). There are times in which the ball gets stuck to the car - I tried to fix this by adding an "epsilon value" to the distance the ball moves back to get it out of collision range, yet it didn't totally fix the bug. It works fine the majority of the time.
    * The velocity of the car affects the trajectory/velocity of the ball, this can be seen in the collision code.
    * The car drives in a realistic fashion similar to the one described in the technical background (line 60). I define a dx/dy by taking the cos/sin of the angle, then update the cars velocity vector based off those values. The turnRate affects the angle in which the car should be facing.
    * When the ball goes into a goal, the position of the car is reset and the ball gets launched from the top-center of the screen at a random velocity.

No extra features this time around, I had a lot of ideas but was unfortunately too busy to implement them =(