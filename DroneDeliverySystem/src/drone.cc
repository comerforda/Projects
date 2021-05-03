#include "src/drone.h"

#include <iostream>

#include "src/json_helper.h"

using csci3081::Drone;

Drone::Drone() {
  AddType<Drone>();
  out_for_delivery_ = 0;
  has_package_ = false;
  current_load_ = 0;
}

Drone::Drone(const picojson::object& val, const entity_project::IGraph* graph)
    : speed_(20.0),
      model_number_("N/A"),
      mass_(50.0),
      max_speed_(60.0),
      base_acceleration_(4.0),
      max_weight_capacity_(10.0),
      base_battery_capacity_(300.0),
      backup_battery_(100.0) {
  AddType<Drone>();
  details_ = val;
  graph_ = graph;
  out_for_delivery_ = 0;
  has_package_ = false;
  current_load_ = 0;
  current_battery_ = base_battery_capacity_;
  InitDetails();
}

void Drone::Update(float dt) {
  if (current_battery_ > 0) {
    Vec3 drone_pos = Vec3(position_);
    Vec3 next_pos = Vec3(direction_);
    Vec3 distance = next_pos - drone_pos;
    if (distance.Length() > radius_) {
      Vec3 dir = distance.Normalize();
      if (physics_model_.compare("force") == 0) {
        Drone::ForceUpdate(dt, dir);
      } else if (physics_model_.compare("acceleration") == 0) {
        Drone::AccelUpdate(dt, dir);
      } else {
        Drone::VelocityUpdate(dt, dir);
      }
    } else {
      if (route_point_ < route_.size() - 1) route_point_ += 1;
      direction_[0] = route_[route_point_][0];
      direction_[1] = route_[route_point_][1];
      direction_[2] = route_[route_point_][2];
    }
    current_battery_ -= dt * 10;
  }
}

void Drone::VelocityUpdate(float dt, Vec3 direction) {
  // Use max velocity as speed for Euler integration
  // Same mathematics as iteration 1
  position_[0] += (direction.x_ * max_speed_ * dt);
  position_[1] += (direction.y_ * max_speed_ * dt);
  position_[2] += (direction.z_ * max_speed_ * dt);
}

void Drone::AccelUpdate(float dt, Vec3 direction) {
  // Calculate a new velocity, update drone's current speed,
  // then perform Euler integration with result.
  float acceleration = GetBaseAcceleration();
  Vec3 velocity;
  velocity.x_ = (direction.x_ * speed_) + (direction.x_ * acceleration * dt);
  velocity.y_ = (direction.y_ * speed_) + (direction.y_ * acceleration * dt);
  velocity.z_ = (direction.z_ * speed_) + (direction.z_ * acceleration * dt);
  if (velocity.Length() <= max_speed_) {
    speed_ = velocity.Length();
    position_[0] += velocity.x_ * dt;
    position_[1] += velocity.y_ * dt;
    position_[2] += velocity.z_ * dt;
  } else {
    // Newly calculated velocity is too high, so use max
    position_[0] += direction.x_ * max_speed_ * dt;
    position_[1] += direction.y_ * max_speed_ * dt;
    position_[2] += direction.z_ * max_speed_ * dt;
  }
}

void Drone::ForceUpdate(float dt, Vec3 direction) {
  // Calculate force, acceleration, and new velocity, update
  // drone's current speed, then perform Euler integration with
  // result.
  float m_drone = GetMass();
  float m_total = m_drone + current_load_;
  float acceleration = GetBaseAcceleration();
  float f_drone = m_drone * acceleration;
  acceleration = f_drone / m_total;
  Vec3 velocity;
  velocity.x_ = (direction.x_ * speed_) + (direction.x_ * acceleration * dt);
  velocity.y_ = (direction.y_ * speed_) + (direction.y_ * acceleration * dt);
  velocity.z_ = (direction.z_ * speed_) + (direction.z_ * acceleration * dt);
  if (velocity.Length() <= max_speed_) {
    speed_ = velocity.Length();
    position_[0] += velocity.x_ * dt;
    position_[1] += velocity.y_ * dt;
    position_[2] += velocity.z_ * dt;
  } else {
    // Newly calculated velocity is too high, so use max
    position_[0] += direction.x_ * max_speed_ * dt;
    position_[1] += direction.y_ * max_speed_ * dt;
    position_[2] += direction.z_ * max_speed_ * dt;
  }
}

void Drone::InitDetails() {
  idle_ = true;
  if (JsonHelper::ContainsKey(details_, "position")) {
    picojson::array json_array = JsonHelper::GetArray(details_, "position");
    for (int i = 0; i < json_array.size(); i++) {
      position_[i] = json_array[i].get<double>();
    }
  } else {
    position_[0] = 0;
    position_[1] = 0;
    position_[2] = 0;
  }

  if (JsonHelper::ContainsKey(details_, "speed"))
    speed_ = JsonHelper::GetDouble(details_, "speed");

  if (JsonHelper::ContainsKey(details_, "direction")) {
    picojson::array json_array = JsonHelper::GetArray(details_, "direction");
    for (int i = 0; i < json_array.size(); i++) {
      direction_[i] = json_array[i].get<double>();
    }
  } else {
    direction_[0] = 1;
    direction_[1] = 0;
    direction_[2] = 0;
  }

  if (JsonHelper::ContainsKey(details_, "radius"))
    radius_ = JsonHelper::GetDouble(details_, "radius");
  else
    radius_ = 1.0;

  if (JsonHelper::ContainsKey(details_, "physics-model"))
    physics_model_ = JsonHelper::GetString(details_, "physics-model");
  else
    // If not otherwise specified, assume model of iteration 1
    physics_model_ = "velocity";

  if (JsonHelper::ContainsKey(details_, "name"))
    name_ = JsonHelper::GetString(details_, "name");
  else
    name_ = "Drone";

  if (JsonHelper::ContainsKey(details_, "model")) {
    attr_.ReadDroneAttributesData();
    model_number_ = JsonHelper::GetString(details_, "model");
    SetAttributes();
  }
  // setup indices of dest_
  dest_.push_back(0);
  dest_.push_back(0);
  dest_.push_back(0);
}

void Drone::SetYPosition(float pos) { position_[1] = pos; }

void Drone::SetAttributes() {
  for (int i = 0; i < attr_.GetSize(); i++) {
    if (model_number_ == attr_.GetModel(i)) {
      mass_ = attr_.GetMass(i);
      max_speed_ = attr_.GetMaxSpeed(i);
      base_acceleration_ = attr_.GetBaseAcceleration(i);
      max_weight_capacity_ = attr_.GetWeightCapacity(i);
      base_battery_capacity_ = attr_.GetBaseBatteryCapacity(i);
      backup_battery_ = base_battery_capacity_;
      break;
    }
  }
}

void Drone::SetRoute() {
  std::unordered_map<std::string, float> ucost;
  std::unordered_map<std::string, bool> uvisited;
  std::unordered_map<std::string, std::string> uparent;
  // retreive a copy of the nodes in the graph
  const std::vector<entity_project::IGraphNode*> nodes = graph_->GetNodes();
  // Clear the route vector
  route_.clear();
  current_route_.clear();
  // Init required vars
  int size = nodes.size();

  std::vector<std::string> fringe;
  Vec3 drone_pos = Vec3(position_);
  Vec3 dest_pos = Vec3(dest_);
  float min = 99999.9;
  float min_dest = 999999.9;
  int min_idx;
  int min_idx_dest;

  // initializing details
  for (int i = 0; i < size; i++) {
    // Find the nearest node to the Drone's position
    Vec3 node_pos = Vec3(nodes[i]->GetPosition());
    Vec3 distance = node_pos - drone_pos;
    if (distance.Length() < min) {
      min = distance.Length();
      min_idx = i;
    }
    // find the closest node to destination
    distance = node_pos - dest_pos;
    if (distance.Length() < min_dest) {
      min_dest = distance.Length();
      min_idx_dest = i;
    }

    ucost[nodes[i]->GetName()] = 99999.9;
    uvisited[nodes[i]->GetName()] = false;
    uparent[nodes[i]->GetName()] = "0";
  }
  // Set our source and destination nodes
  std::string source = nodes[min_idx]->GetName();
  std::string destination = nodes[min_idx_dest]->GetName();

  ucost.at(source) = 0;
  fringe.push_back(source);
  min = 0;
  int min_fringe = 0;
  // implement UCS
  while (fringe.size() > 0) {
    std::string current = fringe[min_fringe];
    const entity_project::IGraphNode* current_node = graph_->GetNode(current);
    fringe.erase(fringe.begin() + min_fringe);

    if (!uvisited.at(current)) uvisited.at(current) = true;
    if (current == destination) break;
    // Get the list of neighbors of current node
    std::vector<entity_project::IGraphNode*> neighbors =
        current_node->GetNeighbors();
    int num_neighbors = neighbors.size();

    // Iterate through the current node's neighbors and update costs
    for (int j = 0; j < num_neighbors; j++) {
      Vec3 dist =
          Vec3(current_node->GetPosition()) - Vec3(neighbors[j]->GetPosition());
      float cost = dist.Length();
      std::string neighbor = neighbors[j]->GetName();

      if (!uvisited.at(neighbor) &&
          ucost.at(current) + cost < ucost.at(neighbor)) {
        // uvisited.at(neighbor) = true;
        fringe.push_back(neighbor);
        ucost.at(neighbor) = ucost.at(current) + cost;
        uparent.at(neighbor) = current;
      }
    }
    // Find the minimum-cost node
    min = 99999.9;
    for (int i = 0; i < fringe.size(); i++) {
      if (ucost.at(fringe[i]) < min) {
        min = ucost.at(fringe[i]);
        min_fringe = i;
      }
    }
  }

  // Add to the route the parents of each node,
  // starting at the destination and ending at the source
  std::string prevNode = uparent.at(destination);
  route_.push_back(dest_);
  current_route_.push_back(destination);
  route_.insert(route_.begin(), graph_->GetNode(destination)->GetPosition());
  while (prevNode != "0") {
    route_.insert(route_.begin(), graph_->GetNode(prevNode)->GetPosition());
    current_route_.insert(current_route_.begin(), prevNode);
    prevNode = uparent.at(prevNode);
  }
  route_.insert(route_.begin(), position_);
  for (int i = 0; i < current_route_.size(); i++) {
    // std::cout << current_route_[i] << std::endl;
  }
  // direction is the next point the drone goes to. Initialize it to the first
  // point in the route.
  // std::vector<float> node_pos = route_[0]->GetPosition();
  direction_[0] = route_[0][0];
  direction_[1] = route_[0][1];
  direction_[2] = route_[0][2];
  route_point_ = 0;
}

void Drone::SetDestination(const std::vector<float>& dest) {
  // dest_ = entity_project::SimpleUMNRoutePoint(dest[0], dest[1], dest[2]);
  dest_[0] = dest[0];
  dest_[1] = dest[1];
  dest_[2] = dest[2];
}

void Drone::DropOff() {
  out_for_delivery_ = 0;
  has_package_ = false;
  idle_ = true;
}

void Drone::PickUp() { has_package_ = true; }

void Drone::AddObserver(entity_project::EntityObserver* observer) {
  observers_.push_back(observer);
}

void Drone::RemoveObserver(entity_project::EntityObserver* observer) {
  for (int i = 0; i < observers_.size(); i++) {
    if (observers_[i] == observer) {
      observers_.erase(observers_.begin() + i);
      break;
    }
  }
}

void Drone::SendNotifications(std::string message) {
  // take out the first and last points in the path
  std::vector<std::vector<float>> path;
  for (int i = 1; i < route_.size() - 1; i++) {
    path.push_back(route_[i]);
  }

  picojson::object notification;
  notification["type"] = picojson::value("notify");
  notification["value"] = picojson::value(message);
  // notification["name"] = picojson::value(name_);
  if (message == "moving") {
    picojson::value pathValue = JsonHelper::EncodeArray(path);
    notification["path"] = pathValue;
  }
  if (message == "updateDetails") {
    if (current_battery_ == base_battery_capacity_) {
      // refilled - change to green
      details_["color"] = picojson::value("0x00ff00");
    } else {
      // dead - change to red
      details_["color"] = picojson::value("0xff0000");
    }
    notification["details"] = picojson::value(details_);
  }
  picojson::value notify(notification);
  for (int i = 0; i < observers_.size(); i++) {
    observers_[i]->OnEvent(notify, *this);
  }
}
