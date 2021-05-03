#include "src/drone_simulation.h"

using csci3081::DroneSimulation;

void DroneSimulation::AddEntity(entity_project::Entity* entity) {
  if (JsonHelper::ContainsKey(entity->GetDetails(), "type")) {
    std::string type = JsonHelper::GetString(entity->GetDetails(), "type");
    if (type == "drone") {
      Drone* drone = entity->AsType<Drone>();
      std::vector<Drone*>& drones = scheduler_.GetDrones();
      entities_.push_back(drone);
    } else if (type == "customer") {
      Customer* customer = entity->AsType<Customer>();
      entities_.push_back(customer);
    } else if (type == "package") {
      Package* pkg = entity->AsType<Package>();
      entities_.push_back(pkg);
    } else if (type == "entity") {
      DefaultEntity* default_entity = entity->AsType<DefaultEntity>();
      entities_.push_back(default_entity);
    } else if (type == "recharge") {
      RechargeDrone* recharge_drone = entity->AsType<RechargeDrone>();
      entities_.push_back(recharge_drone);
    } else {
      DefaultEntity* default_entity = entity->AsType<DefaultEntity>();
      entities_.push_back(default_entity);
    }
  } else {
    DefaultEntity* default_entity = entity->AsType<DefaultEntity>();
    entities_.push_back(default_entity);
  }
}

void DroneSimulation::ScheduleDelivery(entity_project::Package* package,
                                       entity_project::Customer* dest,
                                       const picojson::object& details) {
  Package* pkg = package->AsType<Package>();
  Customer* customer = dest->AsType<Customer>();
  pkg->SetDynamic(true);
  Drone* drone;
  RechargeDrone* recharge_drone;
  std::string type;
  for (int i = 0; i < entities_.size(); i++) {
    if (JsonHelper::ContainsKey(entities_[i]->GetDetails(), "type")) {
      type = JsonHelper::GetString(entities_[i]->GetDetails(), "type");
      if (type == "recharge") {
        recharge_drone = entities_[i]->AsType<RechargeDrone>();
      }
    }
  }
  for (int i = 0; i < entities_.size(); i++) {
    if (JsonHelper::ContainsKey(entities_[i]->GetDetails(), "type")) {
      type = JsonHelper::GetString(entities_[i]->GetDetails(), "type");
      if (type == "drone") {
        drone = entities_[i]->AsType<Drone>();
        if (drone->out_for_delivery_ == 0) {
          scheduler_.CreateDelivery(pkg, customer, drone, details,
                                    recharge_drone);
          pkg->SendNotifications("scheduled");
          if (drone->idle_) {
            drone->SendNotifications("moving");
            drone->idle_ = false;
          }
          break;
        }
      }
    }
  }
}

void DroneSimulation::Update(float dt) {
  for (int i = 0; i < scheduler_.GetDeliveries().size(); i++) {
    scheduler_.GetDeliveries()[i]->Update(dt);
  }
  // Removing package from entities vector if it's delivered.
  for (int i = 0; i < entities_.size(); i++) {
    if (JsonHelper::ContainsKey(entities_[i]->GetDetails(), "type")) {
      std::string type =
          JsonHelper::GetString(entities_[i]->GetDetails(), "type");
      if (type == "package") {
        Package* temp_pkg = entities_[i]->AsType<Package>();
        if (temp_pkg->GetReceived()) {
          for (int i = 0; i < temp_pkg->GetObservers().size(); i++) {
            temp_pkg->RemoveObserver(temp_pkg->GetObservers()[i]);
          }
          delete temp_pkg;
          entities_.erase(entities_.begin() + i);
        }
      }
    }
  }
}

void DroneSimulation::AddObserver(entity_project::Entity* entity,
                                  entity_project::EntityObserver* observer) {
  Package* pkg = entity->AsType<Package>();
  if (pkg) {
    pkg->AddObserver(observer);
  }
  Drone* drone = entity->AsType<Drone>();
  if (drone) {
    drone->AddObserver(observer);
  }
}

void DroneSimulation::RemoveObserver(entity_project::Entity* entity,
                                     entity_project::EntityObserver* observer) {
  Package* pkg = entity->AsType<Package>();
  if (pkg) {
    pkg->RemoveObserver(observer);
  }
  Drone* drone = entity->AsType<Drone>();
  if (drone) {
    drone->RemoveObserver(observer);
  }
}
