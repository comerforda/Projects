/**
    Adam Comerford
    CSci-4611 Assignment #1 Text Rain
**/


import processing.video.*;

// Global variables for handling video data and the input selection screen
String[] cameras;
Capture cam;
Movie mov;
PImage inputImage;
boolean inputMethodSelected = false;
//A poem by Langston Hughes
String words = "Let the rain kiss you Let the rain beat upon your head with silver liquid drops Let the rain sing you a lullaby The rain makes still pools on the sidewalk The rain makes running pools in the gutter The rain plays a little sleep song on our roof at night And I love the rain.";
Letter[] letters;    //array of Letter objects
int threshold = 128; //default threshold
int debugToggle = 0; //for entering debug mode
int numDisplayed = 1;  //number of letters currently being displayed
long drawCount = 0;  //number of times draw() has been called, used to introduce new letters


void setup() {
  
  size(1280, 720);  
  inputImage = createImage(width, height, RGB);
  textFont(loadFont("BodoniMT-Bold-32.vlw"));
  int velocity;
  int i = 0;
  letters = new Letter[words.length()]; //create Letter array
  
  //add letters into array, occasionally initializing words to fall together in a random position
  while (i < words.length()) {
    int wordlength = 0;
    int wordSpot = int(random(inputImage.width/5, (4 * inputImage.width/5)));  //random spot for a word, between 1/6 and 4/5 of display
    velocity = (i % 3) + 1;    //set varying velocities, 1-3 pixels/frame
    
    //randomly decide to add a word (must be at the beginning of word)
    if ((int(random(10) % 3) == 1) && (i > 0) && (words.charAt(i-1) == ' ')) {
      while(words.charAt(i) != ' ') {
        wordlength += (int(textWidth(words.charAt(i)) * 1.5));  //space out the letters of the word
        letters[i] = new Letter(wordlength + wordSpot, 0, velocity, words.charAt(i));
        i++;
      }
    }
    else {
      letters[i] = new Letter(int(random(1280)), 0, velocity, words.charAt(i));  //add individual letter
      i++;
    }
  }
}


void draw() {
  // When the program first starts, draw a menu of different options for which camera to use for input
  // The input method is selected by pressing a key 0-9 on the keyboard
  if (!inputMethodSelected) {
    cameras = Capture.list();
    int y=40;
    text("O: Offline mode, test with TextRainInput.mov movie file instead of live camera feed.", 20, y);
    y += 40; 
    for (int i = 0; i < min(9,cameras.length); i++) {
      text(i+1 + ": " + cameras[i], 20, y);
      y += 40;
    }
    return;
  }

  // This part of the draw loop gets called after the input selection screen, during normal execution of the program.
  // STEP 1.  Load an image, either from a movie file or from a live camera feed. Store the result in the inputImage variable
  if ((cam != null) && (cam.available())) {
    cam.read();
    inputImage.copy(cam, 0,0,cam.width,cam.height, 0,0,inputImage.width,inputImage.height);
    inputImage.filter(GRAY);    //display image as grayscale
    flipImage();
  }
  else if ((mov != null) && (mov.available())) {
    mov.read();
    inputImage.copy(mov, 0,0,mov.width,mov.height, 0,0,inputImage.width,inputImage.height);
    inputImage.filter(GRAY);    //display image as grayscale
    flipImage();
  }

  // Fill in your code to implement the rest of TextRain here..
  //Spacebar pressed - enter debug mode
  if (debugToggle == 1)
    toggleBinary();
    
  //Draw image - everything that follows is text display
  set(0, 0, inputImage);

  //Display threshold in debug mode
  if (debugToggle == 1){
    textAlign(RIGHT);
    fill(240, 0, 0);
    text("Threshold: " + threshold, 1250, 690);
  }
  
  //display the letters over the input image, pause if debug mode is enabled
  if (debugToggle == 0 ){
    for(int i = 0; i < numDisplayed; i++) {
      letters[i].display();
    }
    
    //release a new letter every 10 frames
    if (drawCount % 10 == 0 && numDisplayed < letters.length) 
      numDisplayed++;
    drawCount++;
  }
}


//toggle the binary image of debug mode when spacebar is pressed
void toggleBinary() {
  
  int pixidx;
  inputImage.loadPixels();
    //change background to binary
    for (int x = 0; x < inputImage.width; x++) { 
      for (int y = 0; y < inputImage.height; y++) {
        pixidx = x + y * inputImage.width;
        if (brightness(inputImage.pixels[pixidx]) < threshold)
          inputImage.pixels[pixidx] = color (0);
        if (brightness(inputImage.pixels[pixidx]) > threshold)
          inputImage.pixels[pixidx] = color (255);
      }
    }
    inputImage.updatePixels();
}


//flip the video image to mirror the camera
void flipImage() {
  
  int pixidx;
  color temp;
    inputImage.loadPixels();
    for (int x = 0; x < inputImage.width/2; x++) { 
        for (int y = 0; y < inputImage.height; y++) {
          pixidx = x + y * inputImage.width;
          int oppidx = inputImage.width - 1 - x + y * inputImage.width;
          temp = inputImage.pixels[pixidx];
          inputImage.pixels[pixidx] = inputImage.pixels[oppidx];
          inputImage.pixels[oppidx] = temp;
        }
    }
    inputImage.updatePixels();
}


void keyPressed() {
  
  if (!inputMethodSelected) {
    // If we haven't yet selected the input method, then check for 0 to 9 keypresses to select from the input menu
    if ((key >= '0') && (key <= '9')) { 
      int input = key - '0';
      if (input == 0) {
        println("Offline mode selected.");
        mov = new Movie(this, "TextRainInput.mov");
        mov.loop();
        inputMethodSelected = true;
      }
      else if ((input >= 1) && (input <= 9)) {
        println("Camera " + input + " selected.");           
        // The camera can be initialized directly using an element from the array returned by list():
        cam = new Capture(this, cameras[input-1]);
        cam.start();
        inputMethodSelected = true;
      }
    }
    return;
  }
  // This part of the keyPressed routine gets called after the input selection screen during normal execution of the program
  // Fill in your code to handle keypresses here..
  
  if (key == CODED) {
    if (keyCode == UP) {
      // up arrow key pressed
      threshold += 1;
    }
    else if (keyCode == DOWN) {
      // down arrow key pressed
      threshold -= 1;
    }
  }
  else if (key == ' ') {
    // space bar pressed, enter debugging mode
    if (debugToggle == 0)
      debugToggle = 1;
    else
      debugToggle = 0;
  }  
}


//letter class to keep track of each individual letter's position and display
class Letter {
  
  char letter;
  int x, y; //current position
  int velocity;
  int imgidx; //the pixel of the image the letter is over
  
  //Constructor for letter object
  Letter (int x_, int y_, int velocity_, char letter_) {
    x = x_;
    y = y_;
    velocity = velocity_;
    letter = letter_;
  }
  
  //displays the letter
  void display() {
    fill(0, 250, 250);
    imgidx = int(x + y * inputImage.width); //should be the idx of the pixel we're at
    inputImage.loadPixels();                //need to access pixels
    
    //check if letter is on the screen
    if (y < inputImage.height) {
      //if we've hit an object
      if (brightness(inputImage.pixels[imgidx]) < threshold) {
        
        //move letter up with the objects until it is no longer over black pixel
          while (brightness (inputImage.pixels[imgidx]) < threshold && (y > 0)){
            y = y - 1;
            imgidx = int(x + y * inputImage.width);
          }
          
          //Sloping code - characters will move horizontally if on a slope
          if (brightness(inputImage.pixels[imgidx + 2]) > threshold && brightness(inputImage.pixels[imgidx + 3]) > threshold && (x < 1278)) {
            x = x + 1;
            imgidx = int(x + y * inputImage.width);
          }
          if (brightness(inputImage.pixels[imgidx - 2]) > threshold && brightness(inputImage.pixels[imgidx - 3]) > threshold && (x > 1)) {
            x = x - 1;
            imgidx = int(x + y * inputImage.width);
          } 
          text(letter, x, y);
      }
      
      //haven't hit an object, keep falling
      else {
        int ytemp = y + 1;
        int tempidx = int(x + ytemp * inputImage.width); //should be the idx of the pixel we're at
   
        //checks if any pixel along the letter's path is black before moving it
        while (ytemp < y + velocity) {
          if (ytemp < inputImage.height && brightness(inputImage.pixels[tempidx]) < threshold){ //y + 1 is black, set y to black pixel spot
            y = ytemp;
            break;
          }
          ytemp++;
        }
        if (y != ytemp)    //did not encounter a dark pixel
          y = y + velocity;
        text(letter, x, y);
      }
    }
  }
}
