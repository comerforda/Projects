// I used the code in the Camera in CameraExample.pde then tweaked it to fit my camera needs!
// Changes: I translated all PVectors and their associated methods to our Vec3 library.
//          I changed the initial position of the camera.
//          I disallowed movement in the y direction and modified the movement/turn speed.

class Camera
{
  Camera()
  {
    position      = new Vec3( 430, 200, 50 ); // initial position
    theta         = PI/12; // rotation around Y axis. Starts with forward direction as ( 0, 0, -1 )
    phi           = 0; // rotation around X axis. Starts with up direction as ( 0, 1, 0 )
    moveSpeed     = 150;
    turnSpeed     = 2.0; // radians/sec
    boostSpeed    = 5;  // extra speed boost for when you press shift
    
    // dont need to change these
    shiftPressed = false;
    negativeMovement = new Vec3( 0, 0, 0 );
    positiveMovement = new Vec3( 0, 0, 0 );
    negativeTurn     = new Vec2( 0, 0 ); // .x for theta, .y for phi
    positiveTurn     = new Vec2( 0, 0 );
    fovy             = PI / 4;
    aspectRatio      = width / (float) height;
    nearPlane        = 0.1;
    farPlane         = 10000;
  }
  
  void Update(float dt)
  {
    theta += turnSpeed * ( negativeTurn.x + positiveTurn.x)*dt;
    
    // cap the rotation about the X axis to be less than 90 degrees to avoid gimble lock
    float maxAngleInRadians = 85 * PI / 180;
    phi = min( maxAngleInRadians, max( -maxAngleInRadians, phi + turnSpeed * ( negativeTurn.y + positiveTurn.y ) * dt ) );
    
    // re-orienting the angles to match the wikipedia formulas: https://en.wikipedia.org/wiki/Spherical_coordinate_system
    // except that their theta and phi are named opposite
    float t = theta + PI / 2;
    float p = phi + PI / 2;
    Vec3 forwardDir = new Vec3( sin( p ) * cos( t ),   cos( p ),   -sin( p ) * sin ( t ) );
    Vec3 upDir      = new Vec3( sin( phi ) * cos( t ), cos( phi ), -sin( t ) * sin( phi ) );
    Vec3 rightDir   = new Vec3( cos( theta ), 0, -sin( theta ) );
    Vec3 velocity   = new Vec3( negativeMovement.x + positiveMovement.x, /*negativeMovement.y + positiveMovement.y*/ 0, negativeMovement.z + positiveMovement.z );
    position.add(forwardDir.times( moveSpeed * velocity.z * dt ) );
    position.add(upDir.times(      moveSpeed * velocity.y * dt ) );
    position.add(rightDir.times(   moveSpeed * velocity.x * dt ) );
    position.y = 215;
    aspectRatio = width / (float) height;
    perspective( fovy, aspectRatio, nearPlane, farPlane );
    camera( position.x, position.y, position.z,
            position.x + forwardDir.x, position.y + forwardDir.y, position.z + forwardDir.z,
            upDir.x, upDir.y, upDir.z );
  }
  
  // only need to change if you want difrent keys for the controls
  void HandleKeyPressed()
  {
    if ( key == 'w' || key == 'W' ) positiveMovement.z = 1;
    if ( key == 's' || key == 'S' ) negativeMovement.z = -1;
    if ( key == 'a' || key == 'A' ) negativeMovement.x = -1;
    if ( key == 'd' || key == 'D' ) positiveMovement.x = 1;
    
    if ( key == 'r' || key == 'R' ){
      Camera defaults = new Camera();
      position = defaults.position;
      theta = defaults.theta;
      phi = defaults.phi;
    }
    
    if ( keyCode == LEFT )  negativeTurn.x = 1;
    if ( keyCode == RIGHT ) positiveTurn.x = -0.5;
    if ( keyCode == UP )    positiveTurn.y = 0.5;
    if ( keyCode == DOWN )  negativeTurn.y = -1;
    
  }
  
  // only need to change if you want difrent keys for the controls
  void HandleKeyReleased()
  {
    if ( key == 'w' || key == 'W' ) positiveMovement.z = 0;
    if ( key == 'd' || key == 'D' ) positiveMovement.x = 0;
    if ( key == 'a' || key == 'A' ) negativeMovement.x = 0;
    if ( key == 's' || key == 'S' ) negativeMovement.z = 0;
    
    if ( keyCode == LEFT  ) negativeTurn.x = 0;
    if ( keyCode == RIGHT ) positiveTurn.x = 0;
    if ( keyCode == UP    ) positiveTurn.y = 0;
    if ( keyCode == DOWN  ) negativeTurn.y = 0;
  }
  
  // only necessary to change if you want different start position, orientation, or speeds
  Vec3 position;
  float theta;
  float phi;
  float moveSpeed;
  float turnSpeed;
  float boostSpeed;
  
  // probably don't need / want to change any of the below variables
  float fovy;
  float aspectRatio;
  float nearPlane;
  float farPlane;  
  Vec3 negativeMovement;
  Vec3 positiveMovement;
  Vec2 negativeTurn;
  Vec2 positiveTurn;
  boolean shiftPressed;
};
