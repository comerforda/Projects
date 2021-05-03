#ifndef ENTITY_BASE_H_
#define ENTITY_BASE_H_

#include <EntityProject/entity.h>

/**
 * EntityBase is inherited by each of the Entities - Drone, Package, and
 * Customer. This is where movement logic occurs. It sets the new position of
 * the Entity after it is calculated through Eulerian Integration by Update().
 *
 * This is a theoretical design - not used at the moment
 * but might be in the future.
 */
class EntityBase {
 public:
  /**
   * Not used
   * @preconditions: none
   * @postconditions: none
   */
  EntityBase() {}

  /**
   * EntityBase constructor - not entirely sure what to add here
   * Not used
   * @preconditions: none
   * @postconditions: none
   */
  explicit EntityBase(entity_project::Entity* entity) {}

  /**
   * Sets the new position of an Entity after Integration.
   * Not used
   * @preconditions: none
   * @postconditions: sets each coordinate of position_ to its respective
   *                  value
   */
  void SetPosition(float x, float y, float z) {
    position_[0] = x;
    position_[1] = y;
    position_[2] = z;
  }

  /**
   * Sets the new direction of an Entity after calculating from
   * Not used
   * @preconditions: none
   * @postconditions: sets each coordinate of direction_ to its respective
   *                  value
   */
  void SetDirection(float x, float y, float z) {
    direction_[0] = x;
    direction_[1] = y;
    direction_[2] = z;
  }

  /**
   * Updates the position of the Entity
   * @preconditions: none
   * @postconditions: none
   */
  void Update(float dt) {}

 private:
  /// local variable of position (copy from entity)
  float position_[3];
  /// local direction variable (copy from entity)
  float direction_[3];
};
#endif
