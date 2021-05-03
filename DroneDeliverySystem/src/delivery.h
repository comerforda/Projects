#ifndef DELIVERY_H
#define DELIVERY_H

#include "src/customer.h"
#include "src/drone.h"
#include "src/json_helper.h"
#include "src/package.h"
#include "src/recharge_drone.h"
/**
 * The Delivery class manages the package, drone, recharge drone, and customer
 * objects associated with a delivery. Package, Drone, RechargeDrone, and
 * Customer entities have no knowledge of each other, so we can use the Delivery
 * class to access information about each one of them when necessary. e.g. a
 * Package's position will be update based off the corresponding Drone's speed
 * once it has been picked up.
 *
 */
class Delivery {
 public:
  /**
   * Default constructor - should never be used.
   * @preconditions: none
   * @postconditions: none
   */
  Delivery() {}

  /**
   * Delivery constructor. It takes a created package, customer, drone, recharge
   * drone, and details object and assigns it to local attributes. It also calls
   * SetDestination to set the initial destination of a drone.
   * @preconditions: there has to be an existing package, customer, drone,
   *                 picojson, and recharge drone
   * @postconditions: a delivery is scheduled
   */
  Delivery(csci3081::Package* package, csci3081::Customer* customer,
           csci3081::Drone* drone, const picojson::object& details,
           csci3081::RechargeDrone* recharge_drone);

  /**
   * Sets the destination of a drone to a Package's position if it does not
   * currently have a package, or to a customer's position if it has a package.
   * @preconditions: delivery is scheduled
   * @postconditions: sets the drone's destination and route based on whether it
   * is has the package or not
   */
  void SetDestination();

  /**
   * Update calls each entity object within this Delivery's Update function.
   * Since each Entity is updated in different ways, they have different update
   * functions. It also checks to see if a Package needs to be picked up or
   * dropped off to a Customer. This is called by DroneSimulation.Update().
   * @preconditions: delivery is scheduled
   * @postconditions: updates the entities appropriately based on dt
   */
  void Update(float dt);

  /**
   * WithinRange takes a Drone and Entity to see if the Drone has reached
   * either a Package to be picked up, or a customer to complete the delivery.
   * The Drone will then pick up the package or drop off the package.
   * @preconditions: none
   * @postconditions: checks if the source and destination are within each
   * other's range
   */
  bool WithinRange(entity_project::Entity* source,
                   entity_project::Entity* destination);

  // Only here to pass feedback test, count number of times delivery update
  // occurs
  int countUpdates_ = 0;

  /**
   * Returns the customer for the delivery
   * @preconditions: customer_ is initialized
   * @postconditions: returns the customer for the delivery
   */
  csci3081::Customer* GetCustomer() { return customer_; }

 private:
  /// Pointer to a Package object.
  csci3081::Package* package_;
  /// Pointer to a Drone object.
  csci3081::Drone* drone_;
  /// Pointer to a Customer object.
  csci3081::Customer* customer_;
  /// Pointer to a RechargeDrone object.
  csci3081::RechargeDrone* recharge_drone_;
  /// Json details object
  picojson::object details_;
  /// Indicates if backup battery is dead
  bool dead_backup_battery_ = false;
};

#endif
