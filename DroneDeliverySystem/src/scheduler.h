#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>

#include "src/delivery.h"

/**
 * The Scheduler class keeps track of deliveries and creates new deliveries. It
 * contains member variable deliveries_ which is a vector of pointers to
 * Delivery objects which have been scheduled. Used by DroneSimulation.
 */
class Scheduler {
 public:
  /**
   * Default constructor
   * Not used
   * @preconditions: none
   * @postconditions: none
   */
  Scheduler() {}

  /**
   * Creates a new Delivery object which tracks an associated Package, Customer,
   * Drone, and RechargeDrone. Then it gets put into the deliveries_ vector
   * which keeps track of each scheduled delivery. Called by
   * DroneSimulation.ScheduleDelivery()
   * @preconditions: there has to be an existing package, customer, drone,
   *                 picojson, and recharge drone
   * @postconditions: a delivery is created
   */
  void CreateDelivery(csci3081::Package* package, csci3081::Customer* dest,
                      csci3081::Drone* drone, const picojson::object& details,
                      csci3081::RechargeDrone* recharge_drone);

  /**
   * Returns a reference to a vector containing scheduled deliveries.
   * @preconditions: none
   * @postconditions: returns the list of deliveries
   */
  std::vector<Delivery*>& GetDeliveries() { return deliveries_; }

  /**
   * Returns a reference to a vector containing created drones which can be
   * sent on deliveries.
   * @preconditions: none
   * @postconditions: returns the list of drones
   */
  std::vector<csci3081::Drone*>& GetDrones() { return drones_; }

  /**
   *  Scheduler deconstructor. Deletes each Delivery in deliveries_ vector
   * and each Drone in drones_ vector.
   * @preconditions: Scheduler process has ended
   * @postconditions: deletes all deliveries and drones
   */
  ~Scheduler() {
    for (int i = 0; i < deliveries_.size(); i++) delete deliveries_.at(i);
    for (int i = 0; i < drones_.size(); i++) delete drones_.at(i);
  }

 private:
  /**
   * A vector list of scheduled delivery objects. Can get this list by calling
   * GetDeliveries().
   */
  std::vector<Delivery*> deliveries_;
  /**
   * A vector list of created Drones which are ready to be sent on deliveries.
   * One of these will be passed into a Delivery object during CreateDelivery.
   * The drones are added in DroneSimulation.AddEntity()
   */
  std::vector<csci3081::Drone*> drones_;
};

#endif
