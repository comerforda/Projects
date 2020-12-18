// navier-Stokes fluid-fire simulation based on the paper by Stam (GDC 03)

// ((i)+(n+2)*(j))
Camera camera;

int nx = 30;
int ny = 65;
int nz = 25;
int n = (nx + ny + nz)/3;
int size = (nx+2) * (ny+2) * (nz+2);

// Force field
float[] u = new float[size];
float[] v = new float[size];
float[] w = new float[size];
float[] u_prev = new float[size];
float[] v_prev = new float[size];
float[] w_prev = new float[size];

float[] temp = new float[size];
float[] temp_prev = new float[size];

float[] oxy = new float[size];
float[] fuel = new float[size];

//float temp_damp = 0.991; smoke damp
// 0.96 damp with 0.0000000001 visc
float temp_damp = 0.958;
int scale = 0;
float max_u = 0;
float max_v = 0;

void setup() {
  //size(400, 400);
  size(800, 800, P3D);
  camera = new Camera();
  scale = 5;
  
  for (int i = 0; i < size; i++) {
      u[i] = 0.0;
      v[i] = 0.0;
      w[i] = 0.0;
      u_prev[i] = 0.0;
      v_prev[i] = 0.0;
      w_prev[i] = 0.0;
      temp[i] = 0.0;
      temp_prev[i] = 0.0;
      oxy[i] = 0.0;
      fuel[i] = 0.0;
    }
  
  for (int i = 1; i <= nx; i++) {
    for (int k = 1; k <= nz; k++) {
      temp_prev[IX(i,1,k)] = random(30,45);
    }
  }
  
  for (int i = 1; i < size; i++) {
    v[i] = random(15);
    u[i] = random(-50,40);
    w[i] = random(-50,50);
  }
}


void draw() {
  surface.setTitle("fire sim: " + frameRate);
  camera.Update(1.0/frameRate);
  background(200);
  float max_temp = 0;
  for (int i = 0; i < size; i++) {
    max_temp+=temp[i];
  }
  //println(max_temp);
  // .0001 seems to be the best ballpark val
  float visc = .0000000001;
  float diff = .0000000001;
  float dt = .01;
  vel_step(u, v, w, u_prev, v_prev, w_prev, visc, dt);
  temp_step(temp, temp_prev, u, v, w, diff, dt);
  
  for (int i = nx/5; i < nz-2; i++) {
    for (int k = nx/5; k < nz-2; k++) {
      //temp_prev[IX(i,2,k)] = random(65,70);
      temp_prev[IX(i,1,k)] = random(10,15);
    }
  }
  
  convect(dt);
  drawGrid();
  drawScene();
}

void drawGrid() {
  
  for (int i = 0; i < nx+2; i++) {
    for (int j = 0; j < ny+2; j++) {
      for (int k = 0; k < nz+2; k++) {
      int idx = IX(i,j,k);
      float curr_temp = temp[idx];

      float scaled_temp = 255*atan(curr_temp)/(PI/2);
      float r = scaled_temp;
      float g = scaled_temp;
      float b = scaled_temp;
      if (scaled_temp > 110) {
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
      
      /*
      fill(r,g,b);
      if(scaled_temp > 100) {
        pushMatrix();
        translate(i+438,  height-16-j, 10-k);
        //point(i+340, height-j, 0-k);
        box(0.6);
        popMatrix();
      }
      */
      
      strokeWeight(2);
      stroke(r,g,b);
      if(scaled_temp > 110) {
        point(i+435+random(-0.1,0.1), height-16-j, 0-k+10+random(-.1,.1));
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
  }
}

void convect(float dt) {
  /*
  // Potential for fuel/oxygen here. Only using convectiveness for now.
  float nrgBarrier = 0;
  float rateConstant = 1;
  float exothermicness = 1;
  for (int i = 0; i < n+2; i++) {
    for (int j = 0; j < n+2; j++) {
      float o = oxy[IX(i,j)];
      float f = fuel[IX(i,j)];
      float h = temp[IX(i,j)] + 30;
      
      float reactionRate = (o*f*h - nrgBarrier) * rateConstant;
      oxy[IX(i,j)] -= reactionRate * dt;
      fuel[IX(i,j)] -= reactionRate * dt;
      temp[IX(i,j)] += reactionRate * dt * exothermicness;
     // if (oxy[IX(i,j)] < 0) oxy[IX(i,j)] = 0;
     // if (fuel[IX(i,j)] < 0) fuel[IX(i,j)] = 0;
      if (temp[IX(i,j)] < 0) temp[IX(i,j)] = 0;
    }
  }
  */
  // Convectiveness causes the "rising heat" phenomenon
  float convectiveness = 0.4;
  for (int i = 1; i <= nx; i++) {
    for (int j = 1; j <=ny; j++) {
      for (int k=1; k<=nz; k++) {
        v[IX(i,j,k)] += temp[IX(i,j,k)] * convectiveness;
        
        u[IX(i,j,k)] *= 0.995;
        w[IX(i,j,k)] *= 0.995;
        v[IX(i,j,k)] *= 0.9;
        temp[IX(i,j,k)] *= temp_damp;
        temp_prev[IX(i,j,k)] *= temp_damp;
      }
    }
  }
}

// tempity solver
void temp_step (float[] x, float[] x0, float[] u, float[] v, float[] w, float diff, float dt) {
  add_source ( x, x0, dt );
  SWAP ( x0, x ); 
  diffuse ( 0, x, x0, diff, dt );
  SWAP ( x0, x ); 
  advect ( 0, x, x0, u, v, w, dt );
}

// velocity solver
void vel_step (float[] u, float[] v, float[] w, float[] u0, float[] v0, float[] w0, float visc, float dt) {
  add_source (u, u0, dt ); 
  add_source (v, v0, dt );
  add_source(w, w0, dt);
  SWAP ( u0, u ); 
  diffuse ( 1, u, u0, visc, dt );
  SWAP ( v0, v ); 
  diffuse ( 2, v, v0, visc, dt );
  SWAP(w0, w);
  diffuse(3, w, w0, visc, dt);
  project (u, v, w, u0, v0);
  SWAP ( u0, u ); 
  SWAP ( v0, v );
  SWAP (w0, w);
  advect (1, u, u0, u0, v0, w0, dt ); 
  advect ( 2, v, v0, u0, v0, w0, dt );
  advect ( 3, w, w0, u0, v0, w0, dt );
  project (u, v, w, u0, v0);
}


// advection step - first term in tempity eq.
void advect (int b, float[] d, float[] d0, float[] u, float[] v, float[] w, float dt) {
  int i, j, k, i0, j0, i1, j1, k0, k1;
  float x, y, z, s0, t0, s1, t1, u0, u1, dtx;
  dtx = dt*nx;
  float dty = dt*ny;
  float dtz = dt * nz;

  for ( i=1 ; i<=nx ; i++ ) { 
    for ( j=1 ; j<=ny ; j++ ) { 
      for ( k=1 ; k<=nz ; k++ ) {
        x = i-dtx*u[IX(i,j,k)]; y = j-dty*v[IX(i,j,k)]; z = k-dtz*w[IX(i,j,k)];
        if (x<0.5f) x=0.5f; if (x>nx+0.5f) x=nx+0.5f; i0=(int)x; i1=i0+1;
        if (y<0.5f) y=0.5f; if (y>ny+0.5f) y=ny+0.5f; j0=(int)y; j1=j0+1;
        if (z<0.5f) z=0.5f; if (z>nz+0.5f) z=nz+0.5f; k0=(int)z; k1=k0+1;

        s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1; u1 = z-k0; u0 = 1-u1;
        d[IX(i,j,k)] = s0*(t0*u0*d0[IX(i0,j0,k0)]+t1*u0*d0[IX(i0,j1,k0)]+t0*u1*d0[IX(i0,j0,k1)]+t1*u1*d0[IX(i0,j1,k1)])+
                       s1*(t0*u0*d0[IX(i1,j0,k0)]+t1*u0*d0[IX(i1,j1,k0)]+t0*u1*d0[IX(i1,j0,k1)]+t1*u1*d0[IX(i1,j1,k1)]);
      }
    }
  }
  set_bnd ( n, b, d );
}


// diffuse step - second term of tempity eq.
void diffuse (int b, float[] x, float[] x0, float diff, float dt) {
  int i, j, k, m;
  float a=dt*diff*nx*ny*nz;
  for ( m=0; m<20; m++ ) {
    for ( i=1; i<=nx; i++ ) {
      for ( j=1; j<=ny; j++ ) {
        for (k=1; k<=nz; k++) {
        x[IX(i,j,k)] = (x0[IX(i,j,k)] +a *(x[IX(i+1,j,k)] 
                                              +x[IX(i-1,j,k)] 
                                              +x[IX(i,j+1,k)] 
                                              +x[IX(i,j-1,k)] 
                                              +x[IX(i,j,k+1)] 
                                              +x[IX(i,j,k-1)]))/(1+6*a);
        }
      }
    }
    set_bnd ( n, b, x );
  }
}


// add the source to the tempity field (term 3 of navier-Stokes tempity eq.)
void add_source (float[] x, float[] s, float dt) {
  int i, size=(nx+2)*(ny+2)*(nz+2);
  for ( i=0; i<size; i++ ) x[i] += dt*s[i];
}
// EnD temp SOLVER

// Gauss-Sidel solver
void project(float[] u, float[] v, float[] w, float[] p, float[] div) {
  int i, j, k;
  float h;
  h = 1.0/n;
  for ( i=1; i<=nx; i++ ) {
    for ( j=1; j<=ny; j++ ) {
      for (k = 1; k<=nz; k++) {
        div[IX(i, j,k)] = -(1.0/3.0)*h*(u[IX(i+1, j,k)]
                            -u[IX(i-1, j,k)]
                            +v[IX(i, j+1,k)]
                            -v[IX(i, j-1,k)]
                            +w[IX(i,j,k+1)]
                            -w[IX(i,j,k-1)]);
        p[IX(i, j,k)] = 0;
      }
    }
  }
  set_bnd ( n, 0, div ); 
  set_bnd ( n, 0, p );
  for ( k=0; k<20; k++ ) {
    for ( i=1; i<=n; i++ ) {
      for ( j=1; j<=n; j++ ) {
        for (k=1; k<=nz; k++) {
          p[IX(i,j,k)] = (div[IX(i,j,k)] +(p[IX(i+1,j,k)] 
                                              +p[IX(i-1,j,k)] 
                                              +p[IX(i,j+1,k)] 
                                              +p[IX(i,j-1,k)] 
                                              +p[IX(i,j,k+1)] 
                                              +p[IX(i,j,k-1)]))/6;
        }
      }
    }
    set_bnd ( n, 0, p );
  }
  for ( i=1; i<=nx; i++ ) {
    for ( j=1; j<=ny; j++ ) {
      for (k=1; k<=nz; k++) {
        u[IX(i,j,k)] -= 0.5*(p[IX(i+1, j,k)]-p[IX(i-1, j,k)])/h;
        v[IX(i,j,k)] -= 0.5*(p[IX(i, j+1,k)]-p[IX(i, j-1,k)])/h;
        w[IX(i,j,k)] -= 0.5*(p[IX(i,j, k+1)]-p[IX(i,j, k-1)])/h;
      }
    }
  }
  set_bnd ( n, 1, u ); 
  set_bnd ( n, 2, v );
  set_bnd (n, 3, w);
}


void set_bnd(int n, int b, float[] x) {
  
  for(int i = 1 ; i <= nx ; i++) {
        for(int j = 1 ; j <= ny;j++) {
            x[IX(i,j,0)] = b == 3 ? -x[IX(i,j,1)] : x[IX(i,j,1)];
            x[IX(i,j,nz+1)] = b == 3 ? -x[IX(i,j,nz)] : x[IX(i,j,nz)];
        }
    }
    for(int i = 1; i <= nx ; i++) {
        for(int k = 1 ; k <= nz; k++) {
            x[IX(i,0,k)] = b == 2 ? -x[IX(i,1,k)] : x[IX(i,1,k)];
            x[IX(i,ny+1,k)] = b == 2 ? -x[IX(i,ny,k)] : x[IX(i,ny,k)];
        }
    }
    for(int j = 1; j <= ny; j++) {
        for(int k = 1; k <= nz; k++) {
            x[IX(0,j,k)] = b == 1 ? -x[IX(1,j,k)] : x[IX(1,j,k)];
            x[IX(nx+1,j,k)] = b == 1 ? -x[IX(nx,j,k)] : x[IX(nx,j,k)];
        }
    }
    /*
    for ( int i=1; i<=nx; i++) {
        x[IX(i,  0,  0)] = 1.0/2.0*(x[IX(i,1,  0)]+x[IX(i,  0,  1)]);
        x[IX(i,ny+1,  0)] = 1.0/2.0*(x[IX(i,ny,  0)]+x[IX(i,ny+1,  1)]);
        x[IX(i,  0,nz+1)] = 1.0/2.0*(x[IX(i,0,  nz)]+x[IX(i,  1,nz+1)]);
        x[IX(i,ny+1,nz+1)] = 1.0/2.0*(x[IX(i,ny,nz+1)]+x[IX(i,ny+1,  nz)]);
    }
    
    for ( int i=1; i<=ny; i++) {
        x[IX(0,  i,  0)] = 1.0/2.0*(x[IX(1,i,  0)]+x[IX(0,  i,  1)]);
        x[IX(nx+1,i,  0)] = 1.0/2.0*(x[IX(nx,i,  0)]+x[IX(nx+1,i,  1)]);
        x[IX(0,  i,nz+1)] = 1.0/2.0*(x[IX(0,i,  nz)]+x[IX(1,  i,nz+1)]);
        x[IX(nx+1,i,nz+1)] = 1.0/2.0*(x[IX(nx,i,nz+1)]+x[IX(nx+1,i,  nz)]);
    }
    
    for ( int i=1; i<=nz; i++) {
        x[IX(0,  0,  i)] = 1.0/2.0*(x[IX(0,  1,i)]+x[IX(1,  0,  i)]);
        x[IX(0,  ny+1,i)] = 1.0/2.0*(x[IX(0,  ny,i)]+x[IX(1,  ny+1,i)]);
        x[IX(nx+1,0,  i)] = 1.0/2.0*(x[IX(nx,  0,i)]+x[IX(nx+1,1,  i)]);
        x[IX(nx+1,ny+1,i)] = 1.0/2.0*(x[IX(nx+1,ny,i)]+x[IX(nx,  ny+1,i)]);
    }
    */
    float ratio = (1.0/3.0);
    x[IX(0,0,0)]       = ratio *(x[IX(1,0,0)] +x[IX(0,1,0)] +x[IX(0,0,1)]);
    x[IX(0,ny+1,0)]     = ratio *(x[IX(1,ny+1,0)] +x[IX(0,ny,0)] +x[IX(0,ny+1,1)]);

    x[IX(0,0,nz+1)]     = ratio *(x[IX(1,0,nz+1)] +x[IX(0,1,nz+1)] +x[IX(0,0,nz)]);
    x[IX(0,ny+1,nz+1)]   = ratio *(x[IX(1,ny+1,nz+1)] +x[IX(0,ny,nz+1)] +x[IX(0,ny+1,nz)]);

    x[IX(nx+1,0,0)]     = ratio *(x[IX(nx,0,0)] +x[IX(nx+1,1,0)] +x[IX(nx+1,0,1)]);
    x[IX(nx+1,ny+1,0)]   = ratio *(x[IX(nx,ny+1,0)] +x[IX(nx+1,ny,0)] +x[IX(nx+1,ny+1,1)]);
    
    x[IX(nx+1,0,nz+1)]   = ratio *(x[IX(nx,0,nz+1)] +x[IX(nx+1,1,nz+1)] +x[IX(nx+1,0,nz)]);
    x[IX(nx+1,ny+1,nz+1)] = ratio *(x[IX(nx,ny+1,nz+1)] +x[IX(nx+1,ny, nz+1)] +x[IX(nx+1,ny+1,nz)]);
}


void SWAP(float[] x0, float[] x) {
  float[] temp = new float[size];
  for (int i = 0; i < size; i++) {
    temp[i] = x0[i];
    x0[i] = x[i];
    x[i] = temp[i];
  }
}

int IX(int i, int j, int k) { 
  return i + (nx+2)*j + (nx+2)*(ny+2)*k;
}

void keyPressed()
{
  camera.HandleKeyPressed();
}

void keyReleased()
{
  camera.HandleKeyReleased();
}

void drawScene(){
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
  box(5, 5, 50);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(460, 790, 5);
  rotateZ(PI/12);
  rotateY(-PI/12);
  box(50, 5, 5);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(440, 790, -7);
  rotateZ(-PI/12);
  rotateY(-PI/8);
  box(50, 5, 5);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(440, 790, 5);
  rotateZ(-PI/12);
  rotateY(PI/12);
  box(50, 5, 5);
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
