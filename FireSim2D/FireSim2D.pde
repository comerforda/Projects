// navier-Stokes fluid-fire simulation based on the paper by Stam (GDC 03)
import processing.sound.*;
SoundFile cackle;
SoundFile song;
SoundFile blow;
SoundFile yum, yuck;

void setup() {
  size(800, 800, P3D);
  background(0);
  camera = new Camera();
  noStroke();
  // load models
  sky = loadImage("sky.jpg");
  grass = loadImage("grass.jpg");
  birch = loadShape("BirchTree_3.obj");
  tree = loadShape("CommonTree_1.obj");
  stick = loadShape("Staff.obj");
  stick.setFill(color(80, 50, 50));
  // Load sounds
  cackle = new SoundFile(this, "campfire-1.mp3");
  cackle.play();
  cackle.amp(0.3);
  cackle.loop();
  song = new SoundFile(this, "campfire song.mp3");
  song.play();
  song.loop();
  blow = new SoundFile(this, "blow.mp3");
  yum = new SoundFile(this, "yum.mp3");
  yuck = new SoundFile(this, "yuck.mp3");
  
  firePos = new Vec3(445, height-20, 0);
  mallowPos = new Vec3(390, height-10, 20);
  // Create a Navier-Stokes fire
  campfire = new NS(200, 0.6);
  
  
  for (int i = 5*campfire.n + campfire.n/2; i < 5*campfire.n + campfire.n/2 + 40; i++)
  {
    campfire.temp_prev[i] = random(300,500);
  }

  for (int i = 2*campfire.n; i < campfire.size; i++) {
    campfire.v[i] = random(5);
    campfire.u[i] = random(-2,2);
  }
}


void draw() {
  surface.setTitle("fire sim: " + frameRate);
  background(30, 30, 100);
  fill(255);
  float dt = .01;
  camera.Update(1/frameRate);
  
  float mouseScaleX = mouseX / (width/30) - 15;
  float mouseScaleY = mouseY / (height/40) - 20;
  hor = mouseScaleX;
  vert = mouseScaleY;
  
  campfire.update(dt);
  if (has_mallow == 1)
    updateMallow();
  if (burning_mallow == 1)
    mallow_fire.update(dt);
  drawFire();
  drawScene();
}


void updateMallow() {
  // Update mallow position based on mouse position
  mallowPos = new Vec3(camera.position.x+hor, camera.position.y+vert, camera.position.z-30);
  //println(mallowPos.minus(firePos).length());
  float mallow_distance = mallowPos.minus(firePos).length();
  
  // Update mallow temperature
  if (mallow_distance < 50) mallow_temp += 1;
  else if (mallow_distance < 75) mallow_temp += 0.45;
  else if (mallow_distance < 85) mallow_temp += 0.35;
  else if (mallow_distance < 100) mallow_temp += 0.25;
  else if (mallow_distance < 115) mallow_temp += 0.15;
  if (burning_mallow == 1) mallow_temp += 1;
  if (mallow_temp > 230) mallow_temp = 230; 
  mallow_color = color(230 - 0.8*mallow_temp, 230 - mallow_temp, 230 - mallow_temp);

  if (burning_mallow == 0 && mallow_temp > 150 && blown_out == 0) {
    burning_mallow = 1;
    mallow_fire = new NS(150, 0.1);
    
    for (int i = 2*mallow_fire.n; i < mallow_fire.size; i++) {
      mallow_fire.v[i] = random(5);
      mallow_fire.u[i] = random(-2,2);
    }
    
    for (int i = mallow_fire.n/2-10; i < mallow_fire.n/2+10; i++)
    {
      mallow_fire.temp_prev[i] = random(100,150);
      mallow_fire.temp[i] = random(100,150);
    }
  }
}


void drawFire() {
  for (int i = 5*campfire.n + campfire.n/2; i < 5*campfire.n + campfire.n/2 + 40; i++)
    {
      campfire.temp_prev[i] = random(130,180);
    }
  for (int i = 0; i < campfire.n+2; i++) {
    for (int j = 0; j < campfire.n+2; j++) {
      int idx = campfire.IX(i, j);
      float curr_temp = campfire.temp[idx];
      float scaled_temp = 255*atan(curr_temp)/(PI/2);
      
      // Change color based on temperature
      float r = scaled_temp;
      float g = scaled_temp;
      float b = scaled_temp;
      if (scaled_temp > 100) {
        r = scaled_temp;
        g = 80;
        b = 80;
      }
      if (scaled_temp > 120) {
        r = scaled_temp+90;
        g = (scaled_temp-120)/0.15;
      }
      if (scaled_temp > 180) {
        r = g = 255;
        b = (scaled_temp - 160)/0.1;
      }
      fill(r,g,b);
      if (scaled_temp > 60) {
        /*
        Potential billboarding
        Vec3 center = new Vec3 (450, camera.position.y, 5);
        Vec3 positionVec = (camera.position).minus(center).normalized();
        Vec3 zVec = new Vec3(0,0,1).normalized();
        
        float angle = 0;
        angle = acos(dot(zVec, positionVec));
        if (camera.position.x < 450)
          angle = -acos(dot(zVec, positionVec));
        else
          angle = acos(dot(zVec, positionVec));
        //println(angle);
        */
        pushMatrix();
        translate(i*campfire.scale+384, height - 12 - j*campfire.scale, 5);
        //rotateY(angle);
        square(0,0,campfire.scale);
        //square(i*campfire.scale+340, height - 10 - j*campfire.scale, campfire.scale);
        popMatrix();
      }
      /*
      // Draw velocity field
      float u_val = abs(u[IX(i, j)]);
      float v_val = abs(v[IX(i, j)]);
      float norm =  sqrt(u_val * u_val + v_val * v_val);
       float vel_r = u_val/norm * 255;
       float vel_g = v_val/norm * 255;
       fill(int(vel_r),int(vel_g),0);
       square(i*scale, height - scale - j*scale, scale);
       */
    }
  }
  
  
  // Draw the mallow fire
  if (burning_mallow == 1) {
    //println("x: " + mallowPos.x + " y: " + mallowPos.y + " z: " + mallowPos.z);
    for (int i = mallow_fire.n + mallow_fire.n/2-5; i < mallow_fire.n +mallow_fire.n/2+5; i++)
    {
      mallow_fire.temp_prev[i] = random(80,120);
    }
    for (int i = 0; i < mallow_fire.n+2; i++) {
      for (int j = 0; j < mallow_fire.n+2; j++) {
        int idx = mallow_fire.IX(i, j);
        float curr_temp = mallow_fire.temp[idx];
        //println(curr_temp);
        float scaled_temp = 255*atan(curr_temp)/(PI/2);
        
        // Change color based on temperature
        float r = scaled_temp;
        float g = scaled_temp;
        float b = scaled_temp;
        if (scaled_temp > 100) {
          r = scaled_temp;
          g = 80;
          b = 80;
        }
        if (scaled_temp > 120) {
          r = scaled_temp+90;
          g = (scaled_temp-120)/0.15;
        }
        if (scaled_temp > 180) {
          r = g = 255;
          b = (scaled_temp - 160)/0.1;
        }
        fill(r,g,b);
        
        if (scaled_temp > 60) {
          pushMatrix();
          // The fire is in the center of the grid, have to translate it more left to match the mallow
          // (this way the mallow is in the center of the grid with the fire)
          translate(i*mallow_fire.scale+mallowPos.x - 7.4,
                    mallowPos.y-j*mallow_fire.scale-0.5,
                    mallowPos.z);
          square(0,0, mallow_fire.scale);
          popMatrix();
        }
      }
    }
  }
}

void keyPressed()
{
  if (key == 'e' && has_stick == 0)
    has_stick = 1;
  if (has_stick == 1 && key == 'e')
    has_mallow = 1;
  if (key == 'x'){
    if (sitting == 0)
      sitting = 1;
    else
      sitting = 0;
  }
  camera.HandleKeyPressed();
}

void keyReleased()
{
  camera.HandleKeyReleased();
}

void mousePressed() {
  // eat mallow && reset values
  if (has_mallow == 1 && burning_mallow == 0)
    eatMallow();
  else if (has_mallow == 1 && burning_mallow == 1)
    blowMallow();
}


// Eat that delicious mallow
void eatMallow() {
  fill(240);
  textSize(5);
  has_mallow = 0;
  
    if (mallow_temp < 50)
      yum.play();
    else if (mallow_temp < 100)
      yum.play();
    else if (mallow_temp < 120)
      yum.play();
    else if (mallow_temp < 150)
      yuck.play();
    else {
      yuck.play();
    }
    
    mallowPos = new Vec3(390, height-10, 20);
    mallow_temp = 0;
    mallow_color = color(230);
    blown_out = 0;
}


// Attempt to blow out the mallow
void blowMallow() {
  blow.cue(.3);
  blow.play();
  if (int(random(2)%2) == 0) {
      for (int i = 3 * mallow_fire.n; i < 10*mallow_fire.n; i++) {
      mallow_fire.u[i] += 2;
      mallow_fire.u[i+mallow_fire.n] -= 0.8;
    }
  }
  else {
    burning_mallow = 0;  // turn off burn
    blown_out = 1;
    mallow_fire = null;  // remove pointer
  }
}



// Globals
Camera camera;
PImage sky;
PImage grass;
PShape tree, birch, stick, chair;
float hor = 2;  // for stick movement
float vert = 2; // for stick movement
float mallow_temp = 0;
color mallow_color = color(230);
int burning_mallow = 0;
int has_stick = 0;
int has_mallow = 0;
int sitting = 0;
int blown_out = 0;
int blown_once = 0;
Vec3 firePos;
Vec3 mallowPos;
NS campfire;
NS mallow_fire;
