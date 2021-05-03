#include "src/entity_factory.h"

entity_project::Entity* EntityFactory::CreateEntity(
    const picojson::object& val, const entity_project::IGraph* graph) {
  entity_project::Entity* entity = NULL;
  if (csci3081::JsonHelper::ContainsKey(val, "type")) {
    std::string type = val.find("type")->second.get<std::string>();
    if (type == "drone") {
      entity = new csci3081::Drone(val, graph);
    } else if (type == "customer") {
      entity = new csci3081::Customer(val);
    } else if (type == "package") {
      entity = new csci3081::Package(val);
    } else if (type == "recharge") {
      entity = new csci3081::RechargeDrone(val, graph);
    } else {
      entity = new csci3081::DefaultEntity(val);
    }
  } else {
    entity = new csci3081::DefaultEntity(val);
  }
  return entity;
}
