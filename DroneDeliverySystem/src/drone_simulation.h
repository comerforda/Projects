#ifndef DRONE_SIMULATION_H_
#define DRONE_SIMULATION_H_

#include <EntityProject/ANVIL/drone_delivery_system.h>
#include <EntityProject/entity_console_logger.h>
#include <EntityProject/graph.h>

#include <string>
#include <vector>

#include "src/drone.h"
#include "src/entity_factory.h"
#include "src/json_helper.h"
#include "src/recharge_drone.h"
#include "src/scheduler.h"
namespace csci3081 {

/**
 * The DroneSimulation class inherits from the virtual DroneDeliverySystem
 * facade. It implements the virtual functions and handles calls to each class
 * in the subsystem. It essentially works as a request dispatcher.
 */
class DroneSimulation : public entity_project::DroneDeliverySystem {
 public:
  /**
   * Default constructor. Initializes the team name
   * @preconditions: none
   * @postconditions: teamName_ is initialized
   */
  DroneSimulation() { teamName_ = "Section 20 Team 15"; }

  /**
   * Returns the name of our Team.
   * @preconditions: teamName_ is initialized
   * @postconditions: returns the team name
   */
  const std::string& GetTeamName() const { return teamName_; }

  /**
   * This function calls the entity_factory to create whichever entity is in
   * val, based off the object type. Type can be Drone, Customer, or Package.
   * These are later scheduled for deliveries.
   * @preconditions: an Entity is to be created
   * @postconditions: the Entity is created
   */
  entity_project::Entity* CreateEntity(const picojson::object& val) {
    // Investigate json object that is passed in
    // JsonHelper::PrintEntityDetails(val);

    return entity_factory_.CreateEntity(val, graph_);
  }

  /**
   * Adds an entity to our vector list of entities after casting them to their
   * appropriate type within the csci3081 namespace. This is used to keep
   * track of which entities are currently created and ready to be scheduled
   * for deliveries. Also adds Drones to the Drone_ vector list in Scheduler
   * to be selected for deliveries.
   * @preconditions: the Entity is already created
   * @postconditions: the appropriate entity is created and added to the
   *                  entities_ vector
   */
  void AddEntity(entity_project::Entity* entity);

  /**
   * Sets the local IGraph attribute to the passed in IGraph object.
   * @preconditions: none
   * @postconditions: sets graph_ to the passed-in graph
   */
  void SetGraph(const entity_project::IGraph* graph) { graph_ = graph; }

  /**
   * Returns an IGraph, this is called by Drone to get a Graph to search
   * during SetDestination().
   * @preconditions: graph_ is initialized
   * @postconditions: returns the graph
   */
  const entity_project::IGraph* getGraph() { return graph_; }

  /**
   * Schedules a delivery with a corresponding package, customer, and
   * details object. This calls CreateDelivery in the Scheduler class after
   * casting the entity objects to our namespace.
   * @preconditions: there has to be an existing package, destination, and
                     details
   * @postconditions: a delivery is scheduled and assigned to a drone that
   *                  is not out for delivery
   */
  void ScheduleDelivery(entity_project::Package* package,
                        entity_project::Customer* dest,
                        const picojson::object& details);

  /**
   * Adds an Observer to the targeted entity.
   * @preconditions: the Drone or Package was successfully created
   * @postconditions: an observer for the drone or package is added
   */
  void AddObserver(entity_project::Entity* entity,
                   entity_project::EntityObserver* observer);

  /**
   * Removes an observer from the targeted entity.
   * @preconditions: the drone has delivered its package to the customer
   * @postconditions: the observer for the drone and/or package is removed
   */
  void RemoveObserver(entity_project::Entity* entity,
                      entity_project::EntityObserver* observer);

  /**
   * Returns a vector list of entity objects which have been created for
   * deliveries.
   * @preconditions: none
   * @postconditions: returns the list of entities
   */
  const std::vector<entity_project::Entity*>& GetEntities() const {
    return entities_;
  }

  /**
   * Iterate through each scheduled delivery and update the entities involved
   * in that delivery. This calls the Delivery.Update() function for every
   * delivery in the scheduler_.deliveries_ vector. It is passed a dt value,
   * this is the change in time from the last frame of the simulation.
   *
   * This function also removes packages which have been delivered from the
   * entities_ vector, since that should be checked every time the position
   * updated.
   * @preconditions: each entity's position is initialized
   * @postconditions: each entity's position is updated based on dt
   */
  void Update(float dt);

  /**
   * DroneSimulation deconstructor. Deletes each Entity in entities_ vector.
   * @preconditions: DroneSimulation process has ended
   * @postconditions: deletes all entities
   */
  ~DroneSimulation() {
    for (int i = 0; i < entities_.size(); i++) {
      delete entities_[i];
    }
    entities_.clear();
  }

 private:
  /// Our team's name.
  std::string teamName_;
  /// A vector list of each entity object.
  // Can I make this <csci::3081 Entity*> entities_?
  std::vector<entity_project::Entity*> entities_;
  // std::vector<csci3081::Entity*> entities_;
  /**
   * Used to schedule new deliveries. Can access deliveries through
   * GetDeliveries().
   */
  Scheduler scheduler_;
  /// A factory used to create entity objects.
  EntityFactory entity_factory_;
  /// The Graph with which the Drone traverses.
  const entity_project::IGraph* graph_;
};

}  // namespace csci3081

#endif  // DRONE_SIMULATION_H_
