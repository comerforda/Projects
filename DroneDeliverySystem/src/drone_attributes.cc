#include "src/drone_attributes.h"

void DroneAttributes::ReadDroneAttributesData() {
  std::ifstream fs;
  fs.open("data/planet-x.csv");
  // fs.open("../../repo-iter2-20-16/project/data/planet-x.csv");
  if (!fs.is_open()) {
    std::cout << "Could not open planet-x.csv" << std::endl;
    exit(1);
  }
  char delim = ',';
  int model_index = -1;
  int mass_index = -1;
  int max_speed_index = -1;
  int base_accel_index = -1;
  int weight_cap_index = -1;
  int battery_index = -1;
  std::vector<std::string> row;
  std::string line;
  std::string attr;
  bool isNameRow = true;
  while (!fs.fail()) {
    row.clear();
    std::getline(fs, line);
    std::istringstream iss(line);
    while (std::getline(iss, attr, delim)) {
      row.push_back(attr);
    }
    if (isNameRow) {
      for (int i = 0; i < row.size(); i++) {
        if (row[i] == "Model #") {
          if (model_index == -1) {
            model_index = i;
          }
        } else if (row[i] == "Mass (kg)") {
          if (mass_index == -1) {
            mass_index = i;
          }
        } else if (row[i] == "Max Speed (km/h)") {
          if (max_speed_index == -1) {
            max_speed_index = i;
          }
        } else if (row[i] == "Base Acceleration (m/s^2)") {
          if (base_accel_index == -1) {
            base_accel_index = i;
          }
        } else if (row[i] == "WeightCapacity (kg)") {
          if (weight_cap_index == -1) {
            weight_cap_index = i;
          }
        } else if (row[i] == "Base Battery Capacity (seconds)") {
          if (battery_index == -1) {
            battery_index = i;
          }
        }
      }
      isNameRow = false;
    } else {
      model.push_back(row[model_index]);
      mass.push_back(stof(row[mass_index]));
      max_speed.push_back(stof(row[max_speed_index]));
      base_accel.push_back(stof(row[base_accel_index]));
      weight_capacity.push_back(stof(row[weight_cap_index]));
      base_battery_capacity.push_back(stof(row[battery_index]));
    }
  }
  fs.close();
}
