#include "src/recharge_drone.h"

#include <iostream>

#include "src/json_helper.h"

using csci3081::RechargeDrone;

RechargeDrone::RechargeDrone() {
  AddType<RechargeDrone>();
  out_for_delivery_ = 0;
}

RechargeDrone::RechargeDrone(const picojson::object& val,
                             const entity_project::IGraph* graph)
    : speed_(20.0) {
  AddType<RechargeDrone>();
  details_ = val;
  graph_ = graph;
  out_for_delivery_ = 0;
  InitDetails();
}

void RechargeDrone::Update(float dt) {
  Vec3 drone_pos = Vec3(position_);
  Vec3 next_pos = Vec3(direction_);
  Vec3 distance = next_pos - drone_pos;
  if (distance.Length() > radius_) {
    Vec3 dir = distance.Normalize();
    position_[0] += (dir.x_ * speed_ * dt);
    position_[1] += (dir.y_ * speed_ * dt);
    position_[2] += (dir.z_ * speed_ * dt);
  } else {
    if (route_point_ < route_.size() - 1) route_point_ += 1;
    direction_[0] = route_[route_point_][0];
    direction_[1] = route_[route_point_][1];
    direction_[2] = route_[route_point_][2];
  }
}

void RechargeDrone::InitDetails() {
  if (JsonHelper::ContainsKey(details_, "position")) {
    picojson::array json_array = JsonHelper::GetArray(details_, "position");
    for (int i = 0; i < json_array.size(); i++) {
      position_[i] = json_array[i].get<double>();
      homeBase_.push_back(json_array[i].get<double>());
    }
  } else {
    position_[0] = 0;
    position_[1] = 0;
    position_[2] = 0;
    for (int i = 0; i < 3; i++) {
      homeBase_.push_back(0);
    }
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

  if (JsonHelper::ContainsKey(details_, "name"))
    name_ = JsonHelper::GetString(details_, "name");
  else
    name_ = "Recharge";

  // setup indices of dest_
  dest_.push_back(0);
  dest_.push_back(0);
  dest_.push_back(0);
}

void RechargeDrone::SetRoute() {
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
  direction_[0] = route_[0][0];
  direction_[1] = route_[0][1];
  direction_[2] = route_[0][2];
  route_point_ = 0;
}

void RechargeDrone::SetDestination(const std::vector<float>& dest) {
  // dest_ = entity_project::SimpleUMNRoutePoint(dest[0], dest[1], dest[2]);
  dest_[0] = dest[0];
  dest_[1] = dest[1];
  dest_[2] = dest[2];
}
