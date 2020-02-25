Adam Comerford  
comer059  
2/3/2020  

# Assignment 1 - Text Rain

## Implemented Features
This program should (hopefully) correctly implement all criteria through the "A" range,
excepting the case of frame independent movement (animation based off time, not display() calls) as
I could not get that to work properly.
I believe that I have sufficiently accounted for rain not passing through a 1-pixel-wide black spot,
but found this difficult to test. This case can be seen at the bottom of my Letter.display() function (line 248).
  

I create a Letter object to keep track of each letter's (x, y) position and velocity, as well as the
function to display each letter. The Letter object is located at the bottom of the source code, and it
is where all my Letter display logic is located.  
The program doesn't pick characters from the string to display at random, but rather assigns each of them 
a random x position to display at.

## Word Algorithm
The program pseudo-randomly picks words from the poem "April Rain" by Langston Hughes to display by first 
taking a random integer 1-10 and modding it with 3. If that passes, it checks if the current position
in the Letters array is the beginning of a word. It does so by checking if the previous character is a space. 
It will then set the x value of each letter in the word to a position adjacent to the previous letter such 
that when each is displayed, it forms the word.
The code can be seen within setup() starting at line 40.

## Extra (wizardly?) Features
* When debugging mode is toggled, it halts the display and animation of letters until toggled off. It also 
 displays the current value of the threshold in the bottom-right corner.
* The letters will fall to the right/left if they are sitting on a reasonable slope. This works by checking the
  left/right pixels while sitting on an object.
