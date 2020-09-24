//Vector Library [2D]
//CSCI 5611 Vector 3 Library [Incomplete]

//Instructions: Add 3D versions of all of the 2D vector functions
//              Vec3 must also support the cross product.
public class Vec3 {
  public float x, y, z;
  
  public Vec3(float x, float y, float z){
    this.x = x;
    this.y = y;
    this.z = z;
  }
  
  public String toString(){
    return "(" + x + ", " + y + ", " + z + ")";
  }
  
  public float length(){
    return sqrt(x*x + y*y + z*z);
  }
  
  public float lengthSqr(){
    return x*x + y*y + z*z;
  }
  
  public Vec3 plus(Vec3 rhs){
    return new Vec3(x+rhs.x, y+rhs.y, z+rhs.z);
  }
  
  public void add(Vec3 rhs){
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }
  
  public Vec3 minus(Vec3 rhs){
    return new Vec3(x-rhs.x, y-rhs.y, z-rhs.z);
  }
  
  public void subtract(Vec3 rhs){
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }
  
  public Vec3 times(float rhs){
    return new Vec3(x*rhs, y*rhs, z*rhs);
  }
  
  public void mul(float rhs){
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }
  
  public void normalize(){
    float mag = length();
    x = x/mag;
    y = y/mag;
    z = z/mag;
  }
  
  public Vec3 normalized(){
    float mag = length();
    return new Vec3(x/mag, y/mag, z/mag);
  }
  
  //If the vector is longer than maxL, shrink it to be maxL otherwise do nothing
  public void clampToLength(float maxL){
    float mag = length();
    if (mag > maxL){
      float ratio = maxL/mag;
      mul(ratio);
    }
  }
  
  //Grow or shrink the vector have a length of maxL
  public void setToLength(float newL){
    float ratio = newL/length();
    mul(ratio);
  }
  
  public float distanceTo(Vec3 rhs){
    Vec3 distance = minus(rhs);
    return sqrt(distance.x * distance.x + distance.y * distance.y + distance.z * distance.z);
  }
}

Vec3 interpolate(Vec3 a, Vec3 b, float t){
  return a.plus((b.minus(a)).times(t));
}

float dot(Vec3 a, Vec3 b){
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 cross(Vec3 a, Vec3 b){
  float i = a.y * b.z - a.z * b.y;
  float j = a.z * b.x - a.x * b.z;
  float k = a.x * b.y - a.y * b.x;
  return new Vec3(i,j,k);
}

Vec3 projAB(Vec3 a, Vec3 b){
  return b.times(dot(a,b));
}
