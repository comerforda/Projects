#include "src/scheduler.h"

void Scheduler::CreateDelivery(csci3081::Package* package,
                               csci3081::Customer* dest, csci3081::Drone* drone,
                               const picojson::object& details,
                               csci3081::RechargeDrone* recharge_drone) {
  drone->out_for_delivery_ = 1;
  deliveries_.push_back(
      new Delivery(package, dest, drone, details, recharge_drone));
}
