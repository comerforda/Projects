#ifndef CUSTOMER_H_
#define CUSTOMER_H_

#include <EntityProject/ANVIL/customer.h>

#include "src/json_helper.h"
namespace csci3081 {
/**
 * A Customer receives a Package from a Drone. Inherits from the
 * Customer base class which itself inherits from the Entity base class.
 * It has ID, name, position, direction, radius, and picojson details
 * as inherited attributes.
 */
class Customer : public entity_project::Customer {
 public:
  /**
   * Default constructor
   * @preconditions: the entity that is to be created should be of type Customer
   * @postconditions: an entity created is of type Customer with its details
   * initialized to default values
   */
  Customer() { AddType<Customer>(); }

  /**
   * Constructs a Customer with a specific value to be added to the Entity's
   * details_ attribute
   * @preconditions: the entity that is to be created should be of type Customer
   *                 with a picojson passed in
   * @postconditions: the entity created is of type Customer with its details
   * set to the picojson
   */
  explicit Customer(const picojson::object& val);

  /**
   * Sets the initial position to the Json object's position value.
   * @preconditions: a Customer was successfully created
   * @postconditions: initializes all details according to details_ or default
   * values
   */
  void InitDetails();

  /**
   * This will be used to update the customers position if we ever need to.
   * This is left unimplemented for now, but may be done in future iterations.
   * @preconditions: none
   * @postconditions: none
   */
  void Update(float dt) {}

  /**
   * Overrides isDynamic function in Entity.h
   * @precondition: dynamic_ is initialized
   * @postconditions: returns if the Customer is dynamic or not
   */
  bool IsDynamic() { return dynamic_; }

  /**
   * Sets the truth value of dynamic_
   * @preconditions: val is of type bool
   * @postconditions: dynamic_ is set to val
   */
  bool SetDynamic(bool val) { dynamic_ = val; }

 private:
  /// dynamic condition - is it moving?
  bool dynamic_;
};

}  // namespace csci3081

#endif
