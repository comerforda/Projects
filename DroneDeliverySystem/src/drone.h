#ifndef DRONE_H_
#define DRONE_H_

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
 * A Drone which carries a Package to its associated Customer. Inherits from the
 * Drone base class which itself inherits from the Entity base class.
 * It has ID, name, position, direction, radius, and picojson details
 * as inherited attributes.
 */
class Drone : public entity_project::Drone {
 public:
  /**
   * Default constructor
   * @preconditions: the entity that is to be created should be of type Drone
   * @postconditions: an entity created is of type Drone with its details
   * initialized to default values
   */
  Drone();

  /**
   * Constructs a Drone with a specific value to be added to the Entity's
   * details_ attribute. Also sets the initial position of the drone via
   * SetPosition().
   * @preconditions: the entity that is to be created should be of type Drone
   *                 with a picojson passed in
   * @postconditions: the entity created is of type Drone with its details set
   * to the picojson
   */
  explicit Drone(const picojson::object& val,
                 const entity_project::IGraph* graph);

  /**
   * Updates the position of the drone using one of three physics models.
   * @preconditions: the drone's position is initialized
   * @postconditions: the drone's position is updated based on dt
   */
  void Update(float dt);

  /**
   * Computes new position using velocity model
   * @preconditions: picojson does not have "force" or "acceleration" component
   * @postconditions: the drone's position is updated based on the direction and
   * speed
   */
  void VelocityUpdate(float dt, Vec3 direction);

  /**
   * Computes new position using acceleration model
   * @preconditions: picojson has an "acceleration" component
   * @postconditions: updates the drone's position based on the acceleration
   * obtained by the velocity based on the direction
   */
  void AccelUpdate(float dt, Vec3 direction);

  /**
   * Computes new position using force model
   * @preconditions: picojson has a "force" component
   * @postconditions: updates the drone's position based on the drone's force
   */
  void ForceUpdate(float dt, Vec3 direction);

  /**
   * Sets the drone's values with values in details_ If it does not contain a
   * value, make a default one.
   * @preconditions: a Drone was successfully created
   * @postconditions: initializes all details according to details_ or default
   * values
   */
  void InitDetails();

  /**
   * Uses the UCS algorithm to find the shortest route to its destination
   * (package or customer)
   * @preconditions: drone needs to be scheduled for a delivery
   * @postconditions: finds shortest path to destination (package or customer)
   */
  void SetRoute();

  /**
   * Sets the local destination attribute to dest
   * @preconditions: dest is within the scope of the dimension bounds
   * @postconditions: sets dest_ to dest
   */
  void SetDestination(const std::vector<float>& dest);

  /**
   * Marks a specific package as being picked up
   * by this drone. (Package.IsDynamic = true)
   * @preconditions: drone is within the radius of the package
   * @postonditions: drone has picked up package
   */
  void PickUp();

  /**
   * Marks the package this drone is carrying as dropped
   * off. This sets out_for_delivery_ to false, Package.IsDynamic to false,
   * and package.Received to true. This is called in Delivery.
   * @preconditions: drone has package and is within the radius of the customer
   * @postconditions: drone has delivered the package to the customer
   */
  void DropOff();

  /**
   * Returns bool has_package_, signifying if it is carrying a package.
   * @preconditions: has_package_ is initialized
   * @postconditions: returns if the drone has the package
   */
  bool HasPackage() { return has_package_; }

  /**
   * Gets the current route
   * @preconditions: current_route_ is initialized
   * @postconditions: returns the current route of the drone
   */
  const std::vector<std::string>& GetCurrentRoute() const {
    return current_route_;
  }

  /**
   * Gets the remaining battery life
   * @preconditions: current_battery_ is initialized
   * @postconditions: returns the current battery of the drone
   */
  float GetRemainingBattery() const { return current_battery_; }

  /**
   * Gets the current speed
   * @preconditions: speed_ is initialized
   * @postconditions: returns the current speed of the drone
   */
  float GetCurrentSpeed() const { return speed_; }

  /**
   * Gets the max speed
   * @preconditions: max_speed_ is initialized
   * @postconditions: returns the maximum speed of the drone
   */
  float GetMaxSpeed() const { return max_speed_; }

  /**
   * Gets the max carrying capacity
   * @preconditions: max_weight_capacity_ is initialized
   * @postconditions: returns the maximum weight capacity of the drone
   */
  float GetMaxCapacity() const { return max_weight_capacity_; }

  /**
   * Gets the remaining capacity
   * @preconditions: max_weight_capacity_ and current_load_ are initialized
   * @postconditions: returns the remaining carrying capacity of the drone
   */
  float GetRemainingCapacity() const {
    return max_weight_capacity_ - current_load_;
  }

  /**
   * Gets the model number
   * @preconditions: model_number_ is initialized
   * @postconditions: returns the model of the drone
   */
  std::string GetModelNumber() const { return model_number_; }

  /**
   * Gets the mass
   * @preconditions: mass_ is initialized
   * @postconditions: returns the mass of the drone
   */
  float GetMass() const { return mass_; }

  /**
   * Gets the base acceleration
   * @preconditions: base_acceleration_ is initialized
   * @postconditions: returns the base acceleration of the drone
   */
  float GetBaseAcceleration() const { return base_acceleration_; }

  /**
   * Update load when a package is picked up
   * @preconditions: drone picked up package
   * @postconditions: adds the package weight to the current load of the drone
   */
  void AddWeightToLoad(float package_weight) {
    current_load_ += package_weight;
  }

  /**
   * Update load when a package is dropped off
   * @preconditions: drone has delivered package
   * @postconditions: subtracts the package weight from the current load of the
   * drone
   */
  void SubtractWeightFromLoad(float package_weight) {
    current_load_ -= package_weight;
  }

  /**
   * Add an observer to the drones' list of observers
   * @preconditions: a Drone was successfully created
   * @postconditions: an observer for the drone is added
   */
  void AddObserver(entity_project::EntityObserver* observer);

  /**
   * Remove an observer from the drones' list of observers
   * @preconditions: the drone has delivered its package
   * @postconditions: the observer for the package is removed
   */
  void RemoveObserver(entity_project::EntityObserver* observer);

  /**
   * Update a drone's observers
   * @preconditions: an observer for the drone was added
   * @postconditions: updates the drone's status
   */
  void SendNotifications(std::string message);

  /**
   * Return the vector of observers
   * @preconditions: none
   * @postconditions: returns the list of drone observers
   */
  std::vector<entity_project::EntityObserver*> GetObservers() {
    return observers_;
  }

  /**
   * Refills the drone's battery
   * @preconditions: the drone's battery is depleted
   * @postconditions: the drone's battery is set to the maximum battery capacity
   */
  void RefillBattery() { current_battery_ = base_battery_capacity_; }

  /**
   * Tracks whether a Drone has been scheduled for a delivery.
   * This is public as it is used by Scheduler often, avoiding
   * frequent, repeated Get function calls.
   */
  bool out_for_delivery_;

  /**
   * Sets the associated attributes for the drone based on the model
   * @preconditions: picojson has a "model" component
   * @postconditions: sets the drone's properties according to the model number
   */
  void SetAttributes();

  /// tracks if the drone is moving or not
  bool idle_;

  /**
   * Gets the backup battery capacity
   * @preconditions: backup_battery_ is initialized
   * @postconditions: returns how much the backup battery of the drone has
   */
  float GetBackupBattery() { return backup_battery_; }

  /**
   * Modifies the backup battery capacity
   * @preconditions: backup_battery_ is initialized
   * @postconditions: backup_battery_ is updated based on dt
   */
  void SetBackupBattery(float dt) { backup_battery_ -= dt; }

  /**
   * Set y position
   * @preconditions: backup battery has depleted
   * @postconditions: sets the drone's position to the ground
   */
  void SetYPosition(float pos);

 private:
  /// Speed of the drone. Obtain from json value.
  float speed_;
  /// Keeps track if the drone is carrying a package.
  bool has_package_;
  /// Keep track of which point we're on in the route.
  int route_point_;
  /// Model number of the drone. Obtain from json value.
  std::string model_number_;
  /// Mass of the drone.
  float mass_;
  /// Max speed of the drone.
  float max_speed_;
  /// Base acceleration of the drone.
  float base_acceleration_;
  /// Weight capacity of the drone (how much it can carry).
  float max_weight_capacity_;
  /// How much weight is currently being carried
  float current_load_;
  /// Base battery capacity of the drone.
  float base_battery_capacity_;
  /// Current battery capacity of the drone.
  float current_battery_;
  /// Used to get drone attributes based on model_number_
  DroneAttributes attr_;
  /// The route used by the Drone. Positions are stored.
  std::vector<std::vector<float>> route_;
  /// The route used by the Drone. Node names are stored.
  std::vector<std::string> current_route_;
  /// Drone's destination - either a Package or Customer
  std::vector<float> dest_;
  /// The node graph that we obtain our route from
  const entity_project::IGraph* graph_;
  /// Physics model to be used. Obtain from json value.
  std::string physics_model_;
  /// vector of observers
  std::vector<entity_project::EntityObserver*> observers_;
  /// Backup battery capacity of the drone.
  float backup_battery_;
};

}  // namespace csci3081
#endif
