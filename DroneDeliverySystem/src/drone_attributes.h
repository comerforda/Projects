#ifndef DRONE_ATTRIBUTES_H
#define DRONE_ATTRIBUTES_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class DroneAttributes {
 public:
  /**
   * Retrieves drone attributes from csv file
   * @preconditions: Drone has a picojson containing the "model" component
   * @postconditions: saves all of the models and their respective attributes in
   * local vectors
   */
  void ReadDroneAttributesData();

  /**
   * Returns model number at index i
   * @preconditions: there is at least one model saved in the model vector
   * @postconditions: returns the model at index i
   */
  std::string GetModel(int i) { return model[i]; }

  /**
   * Returns the mass at index i
   * @preconditions: there is at least one mass value saved in the mass vector
   * @postconditions: returns the mass at index i
   */
  float GetMass(int i) { return mass[i]; }

  /**
   * Returns the max speed at index i
   * @preconditions: there is at least one max speed value saved in the
   * max_speed vector
   * @postconditions: returns the maximum speed at index i
   */
  float GetMaxSpeed(int i) { return max_speed[i]; }

  /**
   * Returns the base acceleration at index i
   * @preconditions: there is at least one base acceleration value saved in the
   * base_accel vector
   * @postconditions: returns the base acceleration at index i
   */
  float GetBaseAcceleration(int i) { return base_accel[i]; }

  /**
   * Returns the weight capacity at index i
   * @preconditions: there is at least one weight capacity value saved in the
   * weight_capacity vector
   * @postconditions: returns the weight capacity at index i
   */
  float GetWeightCapacity(int i) { return weight_capacity[i]; }

  /**
   * Returns the base battery capacity at index i
   * @preconditions: there is at least one base battery capacity value saved in
   * the base_battery_capacity vector
   * @postconditions: returns the battery capacity at index i
   */
  float GetBaseBatteryCapacity(int i) { return base_battery_capacity[i]; }

  /**
   * Returns the size of the model vector (i.e. how many models are stored)
   * @preconditions: none
   * @postconditions: returns the number of distinct models saved from
   * planet-x.csv
   */
  int GetSize() { return model.size(); }

 private:
  /// Stores all model numbers
  std::vector<std::string> model;

  /// Stores all masses
  std::vector<float> mass;

  /// Stores all max speeds
  std::vector<float> max_speed;

  /// Stores all base accelerations
  std::vector<float> base_accel;

  /// Stores all weight capacities
  std::vector<float> weight_capacity;

  /// Stores all base battery capacities
  std::vector<float> base_battery_capacity;
};

#endif
