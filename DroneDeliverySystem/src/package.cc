#include "src/package.h"

#include <string>
#include <vector>

using csci3081::Package;

Package::Package() {
  AddType<Package>();
  received_ = false;
  dynamic_ = false;
}

Package::Package(const picojson::object& val) {
  AddType<Package>();
  details_ = val;
  received_ = false;
  dynamic_ = false;
  InitDetails();
}

void Package::Update(float dt, const std::vector<float>& drone_pos) {
  position_[0] = drone_pos[0];
  position_[1] = drone_pos[1] - 1.0;
  position_[2] = drone_pos[2];
}

void Package::InitDetails() {
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

  if (JsonHelper::ContainsKey(details_, "radius"))
    radius_ = JsonHelper::GetDouble(details_, "radius");
  else
    radius_ = 1.0;

  if (JsonHelper::ContainsKey(details_, "weight")) {
    weight_ = JsonHelper::GetDouble(details_, "weight");
  } else {
    weight_ = 1.0;
  }
}

void Package::AddObserver(entity_project::EntityObserver* obsever) {
  observers_.push_back(obsever);
}

void Package::RemoveObserver(entity_project::EntityObserver* observer) {
  for (int i = 0; i < observers_.size(); i++) {
    if (observers_[i] == observer) {
      observers_.erase(observers_.begin() + i);
      break;
    }
  }
}

void Package::SendNotifications(std::string message) {
  picojson::object notification;
  notification["type"] = picojson::value("notify");
  notification["value"] = picojson::value(message);
  // cast the picojson::object notification into a picojson::value nofity
  picojson::value notify(notification);
  for (int i = 0; i < observers_.size(); i++) {
    observers_[i]->OnEvent(notify, *this);
  }
}
