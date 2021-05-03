#ifndef ENTITY_OBSERVABLE_H_
#define ENTITY_OBSERVABLE_H_

#include <EntityProject/entity_observer.h>

#include <string>

namespace csci3081 {

class EntityObservable {
 public:
  /**
   * Add an observer to the entity's list of observers
   * Not used
   * @preconditions: none
   * @postconditions: none
   */
  virtual void AddObserver(entity_project::EntityObserver* observer);

  /**
   * Remove an observer from the entity's list of observers
   * Not used
   * @preconditions: none
   * @postconditions: none
   */
  virtual void RemoveObserver(entity_project::EntityObserver* observer);

  /**
   * Update an entity's observers
   * Not used
   * @preconditions: none
   * @postconditions: none
   */
  virtual void SendNotifications(std::string message);
};

}  // namespace csci3081

#endif
