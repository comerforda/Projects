#ifndef DEFAULT_ENTITY_H_
#define DEFAULT_ENTITY_H_
#include <EntityProject/entity.h>

#include "src/json_helper.h"

namespace csci3081 {
/**
 * An entity with no type specified. This is created if no type
 * in the details json object is specified.
 */
class DefaultEntity : public entity_project::Entity {
 public:
  /**
   * Default constructor
   * @preconditions: the entity that is to be created without a specific type
   * @postconditions: an entity created without a specific type
   */
  DefaultEntity() { AddType<DefaultEntity>(); }

  /**
   * Defines a default entity based off val json object
   * @preconditions: the entity that is to be created without a specific type
   *                 with a picojson passed in
   * @postconditions: the entity created without a specific type with its
   * details set to the picojson
   */
  explicit DefaultEntity(const picojson::object& val);

  /**
   * Initializes a default entity's attributes based on it's details_ json
   * object. Makes a default position if one is not present in details_.
   * @preconditions: a DefaultEntity was successfully created
   * @postconditions: initializes all details according to details_ or default
   * values
   */
  void InitDetails();
};

}  // namespace csci3081

#endif
