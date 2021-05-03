#ifndef RECHARGE_DRONE_H
#define RECHARGE_DRONE_H

#include <EntityProject/ANVIL2/drone.h>
#include <EntityProject/entity_observer.h>
#include <EntityProject/graph.h>
#include <EntityProject/simple_UMN_route_manager.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "src/drone_attributes.h"
#include "src/entity_base.h"
#include "src/json_helper.h"
#include "src/vec3.h"

namespace csci3081 {

/**
 * A RechargeDrone which carries a new battery to a dead Drone. Inherits from the
 * Drone base class which itself inherits from the Entity base class.
 * It has ID, name, position, direction, radius, and picojson details
 * as inherited attributes.
 */
class RechargeDrone : public entity_project::Drone {
 public:
  /**
   * Default constructor
   * @preconditions: the entity that is to be created should be of type RechargeDrone
   * @postconditions: an entity created is of type RechargeDrone with its details initialized
   *                  to default values
   */
  RechargeDrone();

  /**
   * Constructs a RechargeDrone with a specific value to be added to the Entity's
   * details_ attribute. Also sets the initial position of the drone via
   * SetPosition().
   * @preconditions: the entity that is to be created should be of type Customer
   *                 with a picojson passed in
   * @postconditions: the entity created is of type Customer with its details set to
   *                  the picojson
   */
  explicit RechargeDrone(const picojson::object& val,
                         const entity_project::IGraph* graph);

  /**
   * Updates the position of the drone using default physics model.
   * @preconditions: the drone's position is initialized
   * @postconditions: the drone's position is updated based on dt
   */
  void Update(float dt);

  /** 
   * Sets the drone's values with values in details_ If it does not contain a
   * value, make a default one.
   * @preconditions: a RechargeDrone was successfully created
   * @postconditions: initializes all details according to details_ or default values
   */
  void InitDetails();

  /**
   * Uses the UCS algorithm to find shortest route to the drone that needs battery
   * @preconditions: delivery drone has run out of battery
   * @postconditions: finds the shortest route to the drone
   */
  void SetRoute();

  /**
   * Sets the local destination attribute to dest
   * @preconditions: dest is within the scope of the dimension bounds
   * @postconditions: sets dest_ to dest
   */
  void SetDestination(const std::vector<float>& dest);

  /**
   * Gets the current route
   * @preconditions: current_route_ is initialized
   * @postconditions: returns the current route of the recharge drone
   */
  const std::vector<std::string>& GetCurrentRoute() const {
    return current_route_;
  }

  /**
   * Gets the remaining (base) battery life (NOT USED)
   * @preconditions: none
   * @postconditions: none
   */
  float GetRemainingBattery() const {}

  /**
   * Gets the current speed
   * @preconditions: speed_ is initialized
   * @postconditions: returns the speed of the recharge drone
   */
  float GetCurrentSpeed() const { return speed_; }

  /**
   * Gets the max carrying capacity (NOT USED)
   * @preconditions: none
   * @postconditions: none
   */
  float GetMaxCapacity() const {}

  /**
   * Gets the remaining capacity (NOT USED)
   * @preconditions: none
   * @postconditions: none
   */
  float GetRemainingCapacity() const {}

  /**
   * Gets the base acceleration (NOT USED)
   * @preconditions: none
   * @postconditions: none
   */
  float GetBaseAcceleration() const {}

  /**
   * Get the homebase_ position
   * @preconditions: homeBase_ is initialized
   * @postconditions: returns the home base of the recharge drone
   */
  std::vector<float> GetHomeBase() { return homeBase_; }

  /**
   * Tracks whether a RechargeDrone has been sent to a dead Drone.
   * This is public as it is used by Scheduler often, avoiding
   * frequent, repeated Get function calls.
   */
  bool out_for_delivery_;

 private:
  /// Speed of the drone. Obtain from json value.
  float speed_;
  /// Keep track of which point we're on in the route.
  int route_point_;
  /// The route used by the Drone. Positions are stored.
  std::vector<std::vector<float>> route_;
  /// The route used by the Drone. Node names are stored.
  std::vector<std::string> current_route_;
  /// Dead drone's destination
  std::vector<float> dest_;
  /// The node graph that we obtain our route from
  const entity_project::IGraph* graph_;
  /// location of the drone's charging station
  std::vector<float> homeBase_;
};

}  // namespace csci3081
#endif
