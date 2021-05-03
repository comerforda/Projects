#ifndef VEC3_H
#define VEC3_H

#include <math.h>

#include <vector>
/**
 * Vec3 is a mathematical 3D vector class used for vector
 * operations such as vector arithmetic and normalization.
 * This will be used for calculating distances and directions
 * when updating the position of Entities.
 */
class Vec3 {
 public:
  /**
   * default constructor. Sets x, y, z to zero.
   * @preconditions: none
   * @postconditions: initialize x_, y_, and z_ to 0
   */
  Vec3() {
    x_ = 0;
    y_ = 0;
    z_ = 0;
  }

  /**
   * Specified Vector constructor, sets x, y, z to inputs.
   * @preconditions: none
   * @postconditions: sets each coordinate to its new respective value
   */
  Vec3(float x, float y, float z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  /**
   * Specified constructor, converts a position array to a new Vec3.
   * @preconditions: none
   * @postconditions: sets each coordinate to its respective value from
   *                  the position array
   */
  explicit Vec3(const float position[]) {
    x_ = position[0];
    y_ = position[1];
    z_ = position[2];
  }

  /**
   * Specified constructor, converts position vector to a new Vec3.
   * @preconditions: none
   * @postconditions: sets each coordinate to its respective value from
   *                  the position vector
   */
  explicit Vec3(const std::vector<float> &position) {
    x_ = position[0];
    y_ = position[1];
    z_ = position[2];
  }

  /**
   * Overloaded Vec3 operator, adds components of rhs.
   * @preconditions: none
   * @postconditions: returns the new Vec3 after updating each coordinate
                      based on passed-in coordinates
   */
  Vec3 operator+(Vec3 const &rhs) {
    float x = x_ + rhs.x_;
    float y = y_ + rhs.y_;
    float z = z_ + rhs.z_;
    return Vec3(x, y, z);
  }

  /**
   * Overloaded Vec3 operator, subtracts components of rhs.
   * @preconditions: none
   * @postconditions: returns the new Vec3 after updating each coordinate
                      based on passed-in coordinates
   */
  Vec3 operator-(Vec3 const &rhs) {
    float x = x_ - rhs.x_;
    float y = y_ - rhs.y_;
    float z = z_ - rhs.z_;
    return Vec3(x, y, z);
  }

  /**
   * normalizes this Vector by dividing each component by magnitude.
   * @preconditions: none
   * @postconditions: returns the new Vec3 after normalizing the current
   *                  vector
   */
  Vec3 Normalize() {
    float magnitude = sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    float x = x_ / magnitude;
    float y = y_ / magnitude;
    float z = z_ / magnitude;
    return Vec3(x, y, z);
  }

  /**
   * Returns the magnitude of a vector.
   * @preconditions: none
   * @postconditions: returns the length of the vector
   */
  float Length() { return sqrt((x_ * x_) + (y_ * y_) + (z_ * z_)); }

  /**
   * Changes a float array to a Vec3
   * @preconditions: none
   * @postconditions: converts the positions array to Vec3 and returns
   *                  the Vec3
   */
  Vec3 PosToVec(const float position[]) {}

  /**
   * Computes the dot product between this Vector and rhs
   * @preconditions: none
   * @postconditions: return the dot product of Vec3 and the passed-in Vec3
   */
  float dot(Vec3 rhs) { return (x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_); }

  /// x component of 3D vector
  float x_;
  /// y component of 3D vector
  float y_;
  /// z component of 3D vector
  float z_;
};

#endif  // End VEC3_H
