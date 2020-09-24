Camera camera;
int maxBoids = 700;
int numBoids = 0;
int maxFire = 3000;
int numFire = 0;
int numMosquitos = 500;
int radius = 5;
// Mosquito Vectors
Vec3 mosqPos[] = new Vec3[numMosquitos];
Vec3 mosqVel[] = new Vec3[numMosquitos];
Vec3 mosqAccel[] = new Vec3[numMosquitos];
// Smoke Vectors
Vec3 pos[] = new Vec3[maxBoids];
Vec3 vel[] = new Vec3[maxBoids];
Vec3 acc[] = new Vec3[maxBoids];
// Fire Vectors
Vec3 firePos[] = new Vec3[maxFire];
Vec3 fireVel[] = new Vec3[maxFire];
Vec3 fireAccel[] = new Vec3[maxFire];
Vec3 center = new Vec3(450, 720, 0);
// Particle Life
float boidLife[] = new float[maxBoids];
float fireLife[] = new float[maxFire];
// Particle/Boid constants
float maxMosqForce = 1000;
float maxMosqSpeed = 50;
float maxSpeed = 100.0;
float maxForce = 40.0;
float maxLife = 5;
float maxFireLife = 1.49;
float fireGenRate = 2000;
float genRate = 140;
// Misc. variables
PFont f;
float r = 0.0;
float theta = 0.0;
float dt = 0;
int fire = 0;
int start = 0;

void setup() {
  size(900, 800, P3D);
  camera = new Camera();
  frameRate(30);          //cap framerate at 30 since nvidia vsync caps it at 30 for any framerate < 60
  f = createFont("Arial",12,false);
  // init mosquitos
  for (int i = 0; i < numMosquitos; i++) {
    mosqPos[i] = new Vec3(random(400, 500), random (700, 750), random(-100, 0));
    mosqVel[i] = new Vec3(random(-1, 1), random(-1,1), random(-1,1));
    mosqVel[i].setToLength(maxMosqSpeed);
  }
}

void draw () {
  background(30,0,80);
  camera.Update(1.0/frameRate);
  DrawScene();
  if (start == 1) {
    
    dt = (1.0/frameRate);
    stroke(150, 150, 75);
    
    for (int i = 0; i < numMosquitos; i++) {
      // Adjust size of mosquitos depending on distance
      if ((mosqPos[i].minus(camera.position)).length() < 350)
        strokeWeight(4);
      else if (mosqPos[i].minus(camera.position).length() < 1000)
        strokeWeight(3);
      else if (mosqPos[i].minus(camera.position).length() < 1750)
        strokeWeight(2);
      else  
        strokeWeight(1);
      point(mosqPos[i].x, mosqPos[i].y, mosqPos[i].z);
    }
    updateMosquitos();
    
    // Creates fire if lit
    if (fire == 1){
      updateFire(dt);
      if (numFire >= maxFire){
        spawnSmoke(dt);
        updateSmoke();
      }
      
      //Draw smoke
      if (center.minus(camera.position).length() < 800)
        strokeWeight(3);
      else
        strokeWeight(2);
      for (int i = 0; i < numBoids; i++){
        if (boidLife[i] < 0.4 * maxLife)
          stroke(170,170,170);
        else if (boidLife[i] < 0.5 * maxLife)
          stroke(140, 140, 140);
        else if (boidLife[i] < 0.65 * maxLife)
          stroke(110, 110, 110);
        else if (boidLife[i] < 0.75 * maxLife)
          stroke(80, 80, 80);
        else if (boidLife[i] < 0.9 * maxLife)
          stroke(50, 50, 50);
        else
          stroke(30, 30, 40);
        point(pos[i].x, pos[i].y, pos[i].z);
        boidLife[i] += dt;
      }
      strokeWeight(3);
      for (int i = 0; i < numFire; i++) {
        stroke(200, 50, 10);
        point(firePos[i].x, firePos[i].y, firePos[i].z);
        fireLife[i] += dt;
      }
    }
  }
}


void updateMosquitos() {
  Vec3 toPlayer = new Vec3 (0,0,0);
  Vec3 toSmoke = new Vec3 (0, 0, 0);
  for (int i = 0; i < numMosquitos; i++) {
    mosqAccel[i] = new Vec3(0,0,0);
    float forceMag = 0.0;
    float distance = 0.0;
    int avgCount = 0;
    int avgVelCount = 0;
    Vec3 avgPos = new Vec3(0, 0, 0);
    Vec3 avgVel = new Vec3(0, 0, 0);
    
    // Begin forces
    for (int j = 0; j < numMosquitos; j++) {
      distance = mosqPos[i].distanceTo(mosqPos[j]);  // calculates distance between current boid & neighbor
      // Seperation force calculation
      if (distance < 15 && distance > .01) {
        Vec3 normal = mosqPos[i].minus(mosqPos[j]).normalized();
        forceMag = mosqVel[i].length()/distance;  //force is a fxn of speed and distance
        normal.setToLength(forceMag);
        normal.clampToLength(maxMosqForce);
        mosqAccel[i] = mosqAccel[i].plus(normal);
      }  // End Seperation Force
      
      // add positions of every nearby neighbor to avg (for cohesion)
      if (distance < 75 && distance > 0.01) {
        avgPos.add(mosqPos[j]);
        avgCount += 1;
      }
      // Add velocities of each nearby neightbor (for alignment)
      if (distance < 50 && distance > 0.01) {
        avgVel.add(mosqVel[j]);
        avgVelCount += 1;
      }
    }
    
    // Cohesion Force calculation, calculate avg pos and apply force
    avgPos.mul(1.0/avgCount);
    if (avgCount > 0) {
      Vec3 cohesionForce = avgPos.minus(mosqPos[i]);
      cohesionForce.clampToLength(15.0);
      mosqAccel[i] = mosqAccel[i].plus(cohesionForce);
    }
    
    // Alignment force calculation, Calculate avg vel and apply force
    // Light alignment force to keep a "swarm-like" behavior, but just enough
    // to keep them organized
    avgVel.mul(1.0/avgVelCount);
    if (avgVelCount >= 1) {
      Vec3 alignment = avgVel.minus(mosqVel[i]).normalized();
      alignment.mul(0.5);
      mosqAccel[i] = mosqAccel[i].plus(alignment);
    }
    
    // If the mosquito swarm is close to the player, move toward them
    toSmoke = avgPos.minus(center);
    toPlayer = (camera.position).minus(avgPos);
    if (toPlayer.length() < 400){
      toPlayer.normalize();
      toPlayer.mul(20.0);
      mosqAccel[i] = mosqAccel[i].plus(toPlayer);
    }
    
    if (toSmoke.length() <= 200 && fire == 1) {
      toSmoke.normalize();
      toSmoke.mul(20);
      mosqAccel[i] = mosqAccel[i].plus(toSmoke);
    }
    
      
  }
  
  
  // update mosquito position
  for (int i = 0; i < numMosquitos; i++) {
    // disallow mosquitos from hitting ground/going too high
    if (mosqPos[i].y + radius >= 775)
      mosqAccel[i].y = -40;
    if (mosqPos[i].y + radius <= 600)
      mosqAccel[i].y = 15;
      
    if (mosqPos[i].x + radius >= 900)
      mosqAccel[i].x = -25;
    if (mosqPos[i].x + radius <= 0)
      mosqAccel[i].x = 25;
      
    if (mosqPos[i].z + radius >= 750)
      mosqAccel[i].z = -25;
    if (mosqPos[i].z + radius <= -750)
      mosqAccel[i].z = 25;
    
    mosqPos[i] = mosqPos[i].plus(mosqVel[i].times(dt));
    mosqVel[i] = mosqVel[i].plus(mosqAccel[i].times(dt));  
    
    if (mosqVel[i].length() > maxMosqSpeed)
        mosqVel[i].setToLength(maxMosqSpeed);
  }
}


void updateFire(float dt) {
  float fireRadius = 20;
  
  float toGenFrac = fireGenRate * dt;
  int toGen = int(toGenFrac);
  float fracPart = toGenFrac - int(toGen);
  if(random(1.0) < fracPart)
    toGen += 1;
  
  for (int i = 0; i < toGen; i++){
     if (numFire >= maxFire)
       for (int j = 0; j < numFire; j++) {
         if (fireLife[j] > maxFireLife){
           r = fireRadius * sqrt(random(1));
           theta = 2 * PI * random(1);
           firePos[j] = new Vec3(r*sin(theta) + 450, random(20) + 775, r*cos(theta));
           fireVel[j] = new Vec3(0, -1, 0);
           fireLife[j] = 0.0;
         }
       }
     else {
       r = fireRadius * sqrt(random(1));
       theta = 2 * PI * random(1);
       firePos[numFire] = new Vec3(r*sin(theta) + 450, random(20) + 775, r*cos(theta));
       fireVel[numFire] = new Vec3(0, -1, 0);
       numFire += 1;
     }
  }
  // Center of fire: (450, 720, 0)
  // Calculate a force to drive the fire toward the center of the fire-ring
  
  for (int i = 0; i < numFire; i++) {
    Vec3 toCenter = center.minus(firePos[i]);
    firePos[i] = firePos[i].plus(fireVel[i].times(dt));
    fireVel[i] = fireVel[i].plus(toCenter.times(dt));
  }
}


// this function calculates the smoke particle system. I started with SJG's particle system example
// modified it to add a "reincarnation" cycle.
void spawnSmoke (float dt) {
  float radius = 8.0;

  float toGenFrac = genRate * dt;
  int toGen = int(toGenFrac);
  float fracPart = toGenFrac - int(toGen);
  if(random(1.0) < fracPart)
    toGen += 1;

  // Spawn new boids until we reach the maximum, then "reincarnate" them
  // if they have outlived their lifespan by resetting their position.
  // Probably not very efficient but it got the job done and produced smooth, infinite animation.
  for (int i = 0; i < toGen; i++) {
    if (numBoids >= maxBoids) 
      for (int j = 0; j < numBoids; j++) {
        if (boidLife[j] > maxLife) {
          r = radius * sqrt(random(1));
          theta = 2 * PI * random(1);
          pos[j] = new Vec3(r*cos(theta) + 450, random(25) + 700, r*sin(theta));
          vel[j] = new Vec3(random(-.3, .3), -1, random(-.2,.2));
          vel[j].normalize();
          vel[j].mul(random(.7*maxSpeed, maxSpeed));
          boidLife[j] = 0.0;
        }
      }
      else {
        r = radius * sqrt(random(1));
        theta = 2 * PI * random(1);
        pos[numBoids] = new Vec3(r*sin(theta) + 450, random(25) + 700,  random(-10,10));
        vel[numBoids] = new Vec3(random(-.3, .3), -1, random(-.2,.2));
        vel[numBoids].normalize();
        vel[numBoids].mul(random(.7*maxSpeed, maxSpeed));
        numBoids += 1;
      }
  }
}

void updateSmoke() {
  // Calculate Flocking forces
  for (int i = 0; i < numBoids; i++) {
    acc[i] = new Vec3(0,0,0);
    float forceMag = 0.0;
    float distance = 0.0;
    
    // Seperation force - based off prof. Guy's Boid2D but modified for my project
    for (int j = 0; j < numBoids; j++) {
      distance = (pos[i]).distanceTo(pos[j]); // calculates distance between current boid & neighbor
      
      if (distance < 8 && distance > .01) {
        Vec3 normalX = new Vec3 (pos[i].x - pos[j].x, -1, pos[i].z - pos[j].z);  //only separate by x component - smoke always rise
        forceMag = vel[i].length()/distance;      //force is a fxn of speed and distance
        normalX.setToLength(forceMag);
        normalX.clampToLength(maxForce);     // prevents ridiculous scaling if distance is low (see forceMag above)
        acc[i] = acc[i].plus(normalX);
      }
    }  // End Seperation force
  
    // Cohesion Force
    int avgCount = 0;
    Vec3 avgPos = new Vec3(0.0,0.0, 0.0);
    // add positions of every nearby neighbor to avg
    for (int j = 0; j < numBoids; j++) {
      distance = (pos[i]).distanceTo(pos[j]);
      if (distance < 100 && distance > 0.01) {
        avgPos.add(pos[j]);
        avgCount += 1;
      }
    }
    // Calculate avg and apply force
    avgPos.mul(1.0/avgCount);
    if (avgCount > 0 && pos[i].y > .3 * height) {    // Stop cohesion near top to simulate smoke dissipation
      Vec3 cohesionForce = avgPos.minus(pos[i]);
      cohesionForce.clampToLength(10.0);
      acc[i] = acc[i].plus(cohesionForce);
    }
    
  }  //End Forces

  // Update position and Velocity - based off Prof. Guy's Boid2D
  for (int i = 0; i < numBoids; i++) {
    pos[i] = pos[i].plus(vel[i].times(dt));
    vel[i] = vel[i].plus(acc[i].times(dt));
    
    if (vel[i].length() > maxSpeed) {
      vel[i].setToLength(maxSpeed); 
    }
  }
}

void DrawScene() {
  if (start == 0) {
    textFont(f, 12);
    fill(255);
    text("Press MB1 to start simulation", 350, 700, 100);
  }
  // Display fire text (rotate it toward camera)
  if (camera.position.minus(center).length() < 300 && fire != 1) {
    float angle = 0;
    Vec3 positionVec = ((camera.position).minus(center)).normalized();
    Vec3 unitVec = new Vec3(0, 0, 1).normalized();
    if (camera.position.x < 450)    //if we're to the left of the fire, negate the angle
      angle = -acos(dot(unitVec,positionVec));
    else
      angle = acos(dot(unitVec,positionVec));
    pushMatrix();
    textFont(f,12);
    fill(255);
    translate(390, 700, 0);
    rotateY(angle);
    text("Light fire? Press MB1", 0, 0, 0);
    popMatrix();
  }
  
  //Display framerate + number of boids
  pushMatrix();
  textFont(f,12);
  fill(255);
  rotateY(PI/4);
  if (fire == 1)
  text("Particles: " + str(numBoids + numFire + numMosquitos) + "   FPS: " + str(frameRate), 1, 600, -200);
  else
    text("Particles: " + str(numMosquitos) + "   FPS: " + str(frameRate), 1, 600, -200);
  popMatrix();
  
  // Draw ground
  fill(50, 100, 50);
  noStroke();
  pushMatrix();
  translate(0, 800, 0);
  box(10000.0, 0.1, 10000.0);  //dimensions match the far plane
  popMatrix();
  
  // Draw logs
  fill(105, 22, 22);
  pushMatrix();
  translate(450, 795, 0);
  box(10, 10, 50);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(460, 790, 5);
  rotateZ(PI/6);
  rotateY(-PI/12);
  box(50, 10, 10);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(440, 790, -7);
  rotateZ(-PI/4);
  rotateY(-PI/8);
  box(50, 10, 7);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(440, 790, 5);
  rotateZ(-PI/6);
  rotateY(PI/12);
  box(50, 10, 10);
  popMatrix();
  
  //draw trees - NW corner
  fill(70, 10, 10);
  pushMatrix();
  translate(0, 700, -1000);
  rotateZ(PI/2);
  box(200, 40, 40);
  popMatrix();
  
  fill(30, 120, 30);
  pushMatrix();
  translate(50, 600, -1000);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(0, 550, -1000);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(-50, 600, -1000);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(0, 600, -950);
  sphere(50);
  popMatrix();
  // SW corner
  fill(70, 10, 10);
  pushMatrix();
  translate(0, 700, 1000);
  rotateZ(PI/2);
  box(200, 40, 40);
  popMatrix();
  
  fill(30, 120, 30);
  pushMatrix();
  translate(50, 600, 1000);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(0, 550, 1000);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(-50, 600, 1000);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(0, 600, 950);
  sphere(50);
  popMatrix();
  // SE corner
  fill(70, 10, 10);
  pushMatrix();
  translate(900, 700, 800);
  rotateZ(PI/2);
  box(200, 40, 40);
  popMatrix();
  
  fill(30, 120, 30);
  pushMatrix();
  translate(850, 600, 800);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(900, 550, 800);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(950, 600, 800);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(900, 600, 750);
  sphere(50);
  popMatrix();
  //NE corner
  fill(70, 10, 10);
  pushMatrix();
  translate(900, 700, -800);
  rotateZ(PI/2);
  box(200, 40, 40);
  popMatrix();
  
  fill(30, 120, 30);
  pushMatrix();
  translate(850, 600, -800);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(900, 550, -800);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(950, 600, -800);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(900, 600, -750);
  sphere(50);
  popMatrix();
  
  // East
  fill(70, 10, 10);
  pushMatrix();
  translate(1000, 700, 0);
  rotateZ(PI/2);
  box(200, 40, 40);
  popMatrix();
  
  fill(30, 120, 30);
  pushMatrix();
  translate(950, 600, 0);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(1000, 550, 0);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(1050, 600, 0);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(1000, 600, -50);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(1000, 600, 50);
  sphere(50);
  popMatrix();
  
   // East
  fill(70, 10, 10);
  pushMatrix();
  translate(-100, 700, 0);
  rotateZ(PI/2);
  box(200, 40, 40);
  popMatrix();
  
  fill(30, 120, 30);
  pushMatrix();
  translate(-150, 600, 0);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(-100, 550, 0);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(-50, 600, 0);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(-100, 600, -50);
  sphere(50);
  popMatrix();
  pushMatrix();
  translate(-100, 600, 50);
  sphere(50);
  popMatrix();
}

// Handle user input
void mouseClicked() {
  if (start == 1){
    if (fire == 0)
      fire = 1;
    else {
      fire = 0;
    }
    return;
  }
  else
    start = 1;
}

void keyPressed()
{
  camera.HandleKeyPressed();
}

void keyReleased()
{
  camera.HandleKeyReleased();
}
