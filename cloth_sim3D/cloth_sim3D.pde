// 5611 Project 2 - Cloth Simulation

int nodesY = 15;
int nodesX = 20;
int num_nodes = nodesY * nodesX;
// Force variables
float kv = 30; //30
float k = 200; //200
float rest_len = 1.5;
float mass = 0.05;
Vec3 gravity = new Vec3(0, 200, 0);
// Node variables
Vec3 pos[][] = new Vec3[nodesX][nodesY];
Vec3 vel[][] = new Vec3[nodesX][nodesY];
Vec3 vel_new[][] = new Vec3[nodesX][nodesY];
Vec3 acc[][] = new Vec3[nodesX][nodesY];
Vec3 air_vel = new Vec3(0, 0, 0);
// et cetera
//Vec3 sphere_pos = new Vec3(400, 235, -30);
Vec3 sphere_pos = new Vec3(415, 220, -15);
float sphere_rad = 8;
PImage cloth;
Camera camera;
int start = 0;

void setup() {
  size (900, 800, P3D);
  camera = new Camera();
  cloth = loadImage("FabricStripy_S.jpg");
  //cloth = loadImage("FabricBlue.jpg");

  // initialize Vec3 arrays and cloth position
  for (int i = 0; i < nodesX; i++) {
    for (int j = 0; j < nodesY; j++) {
      pos[i][j] = new Vec3(0,0, 0);
      pos[i][j].x = 400 + rest_len * j + 0.2*j;
      pos[i][j].y = 200 + rest_len*j - 1 * j;
      pos[i][j].z = 0 - rest_len *i;
      vel[i][j] = new Vec3(0,0,0);
    }
  }
}

void update(float dt) {
  // Reset accelerations, add gravity
  for (int i = 0; i < nodesX; i++) {
    for (int j = 0; j < nodesY; j++) {
      acc[i][j] = new Vec3(0,0,0);
      acc[i][j].add(gravity);
    }
  }

  // Compute hooke's law - vertical nodes
  for (int i = 0; i < nodesX; i++) {
    for (int j = 0; j < nodesY-1; j++) {
      Vec3 e = pos[i][j+1].minus(pos[i][j]);  //vector between nodes
      float e_mag = e.length();
      Vec3 e_norm = e.normalized();
      
      float f_string = -k*(e_mag - rest_len);
      float projVtop = dot(vel[i][j], e_norm);
      float projVbot = dot(vel[i][j+1], e_norm);
      //float f_damp = -kv*(projVtop - projVbot);  this blew it up - why?
      float f_damp = -kv*(projVbot - projVtop);
      
      Vec3 force = e_norm.times(f_string + f_damp);
      acc[i][j].add(force.times(-1.0/mass));
      acc[i][j+1].add(force.times(1.0/mass));
      // coulomb model - scaled friction in the direction of negative v.
      // !! Replace this with air drag !!
      //acc[i][j].add(vel[i][j].times(-0.1));
    }
  }
  // Compute hooke's law - horizontal nodes
  for (int i = 0; i < nodesX-1; i++) {
    for (int j = 0; j < nodesY; j++) {
      Vec3 e = pos[i+1][j].minus(pos[i][j]);  //vector between nodes
      float e_mag = e.length();
      Vec3 e_norm = e.normalized();
      
      float f_string = -0.2*k*(e_mag - rest_len);
      float projVright = dot(vel[i+1][j], e_norm);
      float projVleft = dot(vel[i][j], e_norm);
      float f_damp = -kv*(projVright - projVleft);
      
      Vec3 force = e_norm.times((f_string + f_damp));
      acc[i][j].add(force.times(-1.0/mass));
      acc[i+1][j].add(force.times(1.0/mass));
    }
  }
  
  // Attempt at air velocity
  Vec3 force_aero = new Vec3(0,0,0);
  Vec3 v2_a_n = new Vec3(0,0,0);
  for (int i = 0; i < nodesX-1; i++) {
    for (int j = 0; j < nodesY-1; j++) {
      // Do the top triangle
      Vec3 avg_vel_top = (vel[i][j].plus(vel[i][j+1]).plus(vel[i+1][j])).times(0.3333).minus(air_vel); // - air
      Vec3 n_star_top = cross(pos[i][j+1].minus(pos[i][j]), pos[i+1][j].minus(pos[i][j]));
      v2_a_n = n_star_top.times(avg_vel_top.length()*dot(avg_vel_top, n_star_top)/(2*n_star_top.length()));
      force_aero = v2_a_n.times(-0.1).times(0.3333);
      //add the accel to all 3 points of tri
      acc[i][j].add(force_aero);
      acc[i][j+1].add(force_aero);
      acc[i+1][j].add(force_aero);
      
      // Do the bottom triangle
      Vec3 avg_vel_bot = (vel[i][j+1].plus(vel[i+1][j+1]).plus(vel[i+1][j])).times(0.3333).minus(air_vel); // - air
      Vec3 n_star_bot = cross(pos[i+1][j+1].minus(pos[i][j+1]), pos[i+1][j].minus(pos[i][j+1]));
      v2_a_n = n_star_bot.times(avg_vel_bot.length() * dot(avg_vel_bot, n_star_bot)/(2*n_star_bot.length()));
      force_aero = v2_a_n.times(-0.1).times(0.3333);
      // add accel to all 3 points of tri
      acc[i][j+1].add(force_aero);
      acc[i+1][j+1].add(force_aero);
      acc[i+1][j].add(force_aero);
    }
  }

  // Collision detection with Sphere
  Vec3 dist = new Vec3(0,0,0);
  for (int i = 0; i < nodesX; i++) {
    for (int j = 0; j < nodesY; j++){
      dist = pos[i][j].minus(sphere_pos);
      if (dist.length() < sphere_rad + .8) {
        Vec3 n = dist.normalized();
        Vec3 bounce = projAB(vel[i][j],n);
        vel[i][j].subtract(bounce.times(1.05));
        n.mul(sphere_rad - dist.length() + .9);
        pos[i][j].add(n);
      }
    }
  }
  
  // Eulerian integration
  for (int i = 0; i < nodesX; i++){
    for (int j = 1; j < nodesY; j++) {
      vel[i][j].add(acc[i][j].times(dt));
      pos[i][j].add(vel[i][j].times(dt));
    }
  }
}


void draw() {
  camera.Update(1.0/frameRate);
  background(255,255,255);
  float dt = 1/(50*frameRate);
  if (start == 1)
    for (int i = 0; i < 50; i++) {
      update(dt);
    }
  
  stroke(0,0,0);
  fill(0,0,0);

  // Draw sphere
  stroke(200, 0, 0);
  fill(200, 0, 0);
  pushMatrix();
  translate(sphere_pos.x, sphere_pos.y, sphere_pos.z);
  sphere(sphere_rad);
  popMatrix();
  /*
  noStroke();
  for (int i = 0; i < nodesX-1; i++) {
    for (int j = 0; j < nodesY-1; j++) {
      
      beginShape();
      texture(cloth);
      // vertex( x, y, z, u, v)
      vertex(pos[i][j].x, pos[i][j].y, pos[i][j].z,  //top left
             0, 0);
      vertex(pos[i+1][j].x, pos[i+1][j].y, pos[i+1][j].z, //top right
            cloth.width, 0);
      vertex(pos[i+1][j+1].x, pos[i+1][j+1].y, pos[i+1][j+1].z, //bottom right
            cloth.width, cloth.height);
      vertex(pos[i][j+1].x, pos[i][j+1].y, pos[i][j+1].z,  //bottom left
            0, cloth.height);
      endShape();
    }
  }*/

  // Draw cloth
  noStroke();
  textureMode(NORMAL);
  for (int i = 0; i < nodesX-1; i++) {
    for (int j = 0; j < nodesY-1; j++) {
      
      beginShape();
      texture(cloth);
      // vertex( x, y, z, u, v)
      vertex(pos[i][j].x, pos[i][j].y, pos[i][j].z,  //top left
             float(i)/(nodesX-1), float(j)/(nodesY-1));
      vertex(pos[i+1][j].x, pos[i+1][j].y, pos[i+1][j].z, //top right
            float(i+1)/(nodesX-1), float(j)/(nodesY-1));
      vertex(pos[i+1][j+1].x, pos[i+1][j+1].y, pos[i+1][j+1].z, //bottom right
            float(i+1)/(nodesX-1), float(j+1)/(nodesY-1));
      vertex(pos[i][j+1].x, pos[i][j+1].y, pos[i][j+1].z,  //bottom left
            float(i)/(nodesX-1), float(j+1)/(nodesY-1));
      endShape();
    }
  }
}

void keyPressed()
{
  if (key == ' ')
    start = 1;
  camera.HandleKeyPressed();
}

void keyReleased()
{
  camera.HandleKeyReleased();
}
// Cloth reference - https://opengameart.org/content/fabric-stripes-seamless-texture-with-normalmap
// https://www.onlygfx.com/fabric-textures-jpg/
