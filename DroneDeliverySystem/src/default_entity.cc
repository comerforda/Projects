#include "src/default_entity.h"
using csci3081::DefaultEntity;

void DefaultEntity::InitDetails() {
  position_[0] = 0;
  position_[1] = 0;
  position_[2] = 0;
  if (JsonHelper::ContainsKey(details_, "position")) {
    picojson::array json_array = JsonHelper::GetArray(details_, "position");
    for (int i = 0; i < json_array.size(); i++) {
      position_[i] = json_array[i].get<double>();
    }
  }
}

DefaultEntity::DefaultEntity(const picojson::object& val) {
  AddType<DefaultEntity>();
  details_ = val;
  InitDetails();
}
