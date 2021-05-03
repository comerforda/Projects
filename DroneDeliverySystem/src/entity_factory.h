#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <string>

#include "EntityProject/graph.h"
#include "src/customer.h"
#include "src/default_entity.h"
#include "src/drone.h"
#include "src/json_helper.h"
#include "src/package.h"
#include "src/recharge_drone.h"
/**
 * Concrete factory which creates entities Drone, Package, Customer, or
 * RechargeDrone corresponding to the "type" field of the json object passed
 * into it's member function CreateEntity(). Called by
 * DroneSimulation.CreateEntity().
 */

class EntityFactory {
 public:
  /**
   * Creates a Drone/Package/Customer/RechargeDrone entity based on the "type"
   * field of the json object "val". Called from DroneSimulation.
   * @preconditions: none
   * @postconditions: an entity based on the "type" component in val is created
   */
  entity_project::Entity* CreateEntity(const picojson::object& val,
                                       const entity_project::IGraph* graph);
};

#endif
