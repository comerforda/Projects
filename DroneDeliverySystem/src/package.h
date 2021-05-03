#ifndef PACKAGE_H_
#define PACKAGE_H_

#include <EntityProject/ANVIL/package.h>
#include <EntityProject/entity_observer.h>

#include <string>
#include <vector>

#include "src/entity_base.h"
#include "src/entity_observable.h"
#include "src/json_helper.h"
namespace csci3081 {

/**
 * A Package to be delivered by a Drone to a Customer. Inherits from the
 * Package base class which itself inherits from the Entity base class.
 * It has ID, name, position, direction, radius, and picojson details
 * as inherited attributes.
 *
 */
class Package : public entity_project::Package {
 public:
  /**
   * Default constructor.
   * @preconditions: the entity that is to be created should be of type Package
   * @postconditions: an entity created is of type Package with its details
   * initialized to default values
   */
  Package();

  /**
   * Constructs a Package with a specific value to be added to the Entity's
   * details_ attribute.
   * @preconditions: the entity that is to be created should be of type Package
   *                 with a picojson passed in
   * @postconditions: the entity created is of type Package with its details set
   * to the picojson
   */
  explicit Package(const picojson::object& val);

  /**
   *  Update the Package's position relative to the Drone's position, the Drone
   * Position is passed in as an array so it has no knowledge of the Drone
   * itself. This is called in Delivery and is only called if the Drone is
   * currently carrying the package.
   * @preconditions: package has been picked up by a drone
   * @postconditions: the package should be updated to the drone's position
   */
  void Update(float dt, const std::vector<float>& drone_pos);

  /**
   * Notifies if the package is received - false if en route, true if
   * destination has been reached. This will be used to Notify the observers.
   * Doesn't appear to be used
   * @preconditions: none
   * @postconditions: none
   */
  bool Received() {}

  /**
   * Sets the package's 'received' status to true. Called in Drone.h when the
   * drone drops off the package.
   * @preconditions: customer has received package
   * @postconditions: received_ is set to true to indicate customer has received
   * package
   */
  void SetReceived() { received_ = true; }

  /**
   * Returns the package's 'received' status.
   * @preconditions: received_ is initialized
   * @postconditions: returns received_ to indicate if package is received
   */
  bool GetReceived() { return received_; }

  /**
   * Gets the truth value of dynamic_, overloaded from base Entity.
   * @preconditions: dynamic_ is initialized
   * @postconditions: returns if Package is dynamic or not
   */
  bool IsDynamic() { return dynamic_; }

  /**
   * Sets the truth value of dynamic_
   * @preconditions: val is of type bool
   * @postconditions: dynamic_ is set to val
   */
  bool SetDynamic(bool val) { dynamic_ = val; }

  /**
   * Sets the initial position to the details_ position value, or zero
   * otherwise.
   * @preconditions: a Package was successfully created
   * @postconditions: initializes all details according to details_ or default
   * values
   */
  void InitDetails();

  /**
   * Gets the weight of the Package.
   * @preconditions: weight_ is initialized
   * @postconditions: returns the weight of the Package
   */
  float GetWeight() const { return weight_; }

  /**
   * Add an observer to the packages' list of observers
   * @preconditions: a Package was successfully created
   * @postconditions: an observer for the package is added
   */
  void AddObserver(entity_project::EntityObserver* observer);

  /**
   * Remove an observer from the packages' list of observers
   * @preconditions: the customer received the package
   * @postconditions: the observer for the package is removed
   */
  void RemoveObserver(entity_project::EntityObserver* observer);

  /**
   * Update a package's observers
   * @preconditions: an observer for the package was added
   * @postconditions: updates the package's status
   */
  void SendNotifications(std::string message);

  /**
   * Return the vector of observers
   * @preconditions: none
   * @postconditions: returns the list of package observers
   */
  std::vector<entity_project::EntityObserver*> GetObservers() {
    return observers_;
  }

 private:
  /// Marks the package as received by the customer
  bool received_;
  /// Local override of the isDynamic attribute. Gets set
  /// when the Package is scheduled for delivery.
  bool dynamic_;
  /// weight of package
  int weight_;
  /// vector of observers
  std::vector<entity_project::EntityObserver*> observers_;
};

}  // namespace csci3081
#endif
