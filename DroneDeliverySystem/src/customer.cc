#include "src/customer.h"
using csci3081::Customer;

Customer::Customer(const picojson::object& val) {
  AddType<Customer>();
  details_ = val;
  dynamic_ = false;
  InitDetails();
}

void Customer::InitDetails() {
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
}
