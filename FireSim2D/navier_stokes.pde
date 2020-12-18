public class NS {
  int n;
  int size;
  // Force field
  float[] u;
  float[] v;
  float[] u_prev;
  float[] v_prev;
  
  float[] temp;
  float[] temp_prev;
  
  float[] oxy;
  float[] fuel;
  
  //float temp_damp = 0.991; smoke damp
  public float temp_damp;
  public float scale;
  public float max_u;
  public float max_v;
  
  public NS(int n, float scale) {
    this.n = n;
    size = (n+2) * (n+2);
    this.scale = scale;
    
    u = new float[size];
    v = new float[size];
    u_prev = new float[size];
    v_prev = new float[size];
  
    temp = new float[size];
    temp_prev = new float[size];
    
    oxy = new float[size];
    fuel = new float[size];
    
    //float temp_damp = 0.991; smoke damp
    temp_damp = 0.95;
    max_u = 0;
    max_v = 0;
    
    for (int i = 0; i < size; i++) {
      u[i] = 0.0;
      v[i] = 0.0;
      u_prev[i] = 0.0;
      v_prev[i] = 0.0;
      temp[i] = 0.0;
      temp_prev[i] = 0.0;
      oxy[i] = 0.0;
      fuel[i] = 0.0;
    }
  }
  
  public void update(float dt) {
    float visc = .000000001;
    float diff = .000000001;
    vel_step(u, v, u_prev, v_prev, visc, dt);
    temp_step(temp, temp_prev, u, v, diff, dt);
    convect(dt);
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
    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <=n; j++) {
        v[IX(i,j)] += temp[IX(i,j)] * convectiveness;
        
        u[IX(i,j)] *= 0.99;
        v[IX(i,j)] *= 0.95;
        temp[IX(i,j)] *= temp_damp;
        temp_prev[IX(i,j)] *= temp_damp;
      }
    }
  }
  // temp solver
  public void temp_step (float[] x, float[] x0, float[] u, float[] v, float diff, float dt) {
    add_source ( x, x0, dt );
    SWAP ( x0, x ); 
    diffuse ( 0, x, x0, diff, dt );
    SWAP ( x0, x ); 
    advect ( 0, x, x0, u, v, dt );
  }
  
  // velocity solver
  public void vel_step (float[] u, float[] v, float[] u0, float[] v0, float visc, float dt) {
    add_source (u, u0, dt ); 
    add_source (v, v0, dt );
    SWAP ( u0, u ); 
    diffuse ( 1, u, u0, visc, dt );
    SWAP ( v0, v ); 
    diffuse ( 2, v, v0, visc, dt );
    project (u, v, u0, v0 );
    SWAP ( u0, u ); 
    SWAP ( v0, v );
    advect (1, u, u0, u0, v0, dt ); 
    advect ( 2, v, v0, u0, v0, dt );
    project (u, v, u0, v0 );
  }
  
  
  // advection step - first term in tempity eq.
  public void advect (int b, float[] d, float[] d0, float[] u, float[] v, float dt) {
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
    dt0 = dt*n;
    for ( i=1; i<=n; i++ ) {
      for ( j=1; j<=n; j++ ) {
        x = i-dt0*u[IX(i, j)]; 
        y = j-dt0*v[IX(i, j)];
        if (x<0.5) x=0.5; 
        if (x>n+0.5) x=n+ 0.5; 
        i0=(int)x; 
        i1=i0+1;
        if (y<0.5) y=0.5; 
        if (y>n+0.5) y=n+ 0.5; 
        j0=(int)y; 
        j1=j0+1;
        s1 = x-i0; 
        s0 = 1-s1; 
        t1 = y-j0; 
        t0 = 1-t1;
        d[IX(i, j)] = s0*(t0*d0[IX(i0, j0)]+t1*d0[IX(i0, j1)])+
          s1*(t0*d0[IX(i1, j0)]+t1*d0[IX(i1, j1)]);
      }
    }
    set_bnd ( n, b, d );
  }
  
  
  // diffuse step - second term of tempity eq.
  public void diffuse (int b, float[] x, float[] x0, float diff, float dt) {
    int i, j, k;
    float a=dt*diff*n*n;
    for ( k=0; k<20; k++ ) {
      for ( i=1; i<=n; i++ ) {
        for ( j=1; j<=n; j++ ) {
          x[IX(i, j)] = (x0[IX(i, j)] + a*(x[IX(i-1, j)]+x[IX(i+1, j)]+
            x[IX(i, j-1)]+x[IX(i, j+1)]))/(1+4*a);
        }
      }
      set_bnd ( n, b, x );
    }
  }
  
  
  // add the source to the tempity field (term 3 of navier-Stokes tempity eq.)
  public void add_source (float[] x, float[] s, float dt) {
    int i, size=(n+2)*(n+2);
    for ( i=0; i<size; i++ ) x[i] += dt*s[i];
  }
  // EnD tempITY SOLVER
  
  // Gauss-Sidel solver
  public void project(float[] u, float[] v, float[] p, float[] div) {
    int i, j, k;
    float h;
    h = 1.0/n;
    for ( i=1; i<=n; i++ ) {
      for ( j=1; j<=n; j++ ) {
        div[IX(i, j)] = -0.5*h*(u[IX(i+1, j)]-u[IX(i-1, j)]+
          v[IX(i, j+1)]-v[IX(i, j-1)]);
        p[IX(i, j)] = 0;
      }
    }
    set_bnd ( n, 0, div ); 
    set_bnd ( n, 0, p );
    for ( k=0; k<20; k++ ) {
      for ( i=1; i<=n; i++ ) {
        for ( j=1; j<=n; j++ ) {
          p[IX(i, j)] = (div[IX(i, j)]+p[IX(i-1, j)]+p[IX(i+1, j)]+
            p[IX(i, j-1)]+p[IX(i, j+1)])/4;
        }
      }
      set_bnd ( n, 0, p );
    }
    for ( i=1; i<=n; i++ ) {
      for ( j=1; j<=n; j++ ) {
        u[IX(i, j)] -= 0.5*(p[IX(i+1, j)]-p[IX(i-1, j)])/h;
        v[IX(i, j)] -= 0.5*(p[IX(i, j+1)]-p[IX(i, j-1)])/h;
      }
    }
    set_bnd ( n, 1, u ); 
    set_bnd ( n, 2, v );
  }
  
  
  public void set_bnd(int n, int b, float[] x) {
    int i;
    for (i = 1; i<=n; i++) {
      x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
      x[IX(n + 1, i)] = b == 1 ? -x[IX(n, i)] : x[IX(n, i)];
       x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
       x[IX(i, n + 1)] = b == 2 ? -x[IX(i, n)] : x[IX(i, n)];
    }
  
    x[IX(0, 0 )] = 0.5*(x[IX(1, 0 )]+x[IX(0, 1)]);
    x[IX(0, n+1)] = 0.5*(x[IX(1, n+1)]+x[IX(0, n )]);
    x[IX(n+1, 0 )] = 0.5*(x[IX(n, 0 )]+x[IX(n+1, 1)]);
    x[IX(n+1, n+1)] = 0.5*(x[IX(n, n+1)]+x[IX(n+1, n )]);
  }
  
  
  public void SWAP(float[] x0, float[] x) {
    float[] temp = new float[size];
    for (int i = 0; i < size; i++) {
      temp[i] = x0[i];
      x0[i] = x[i];
      x[i] = temp[i];
    }
  }
  
  int IX(int i, int j) { 
    return (i + (n+2) * j);
  }
}
