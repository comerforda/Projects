#include "src/delivery.h"

Delivery::Delivery(csci3081::Package* package, csci3081::Customer* customer,
                   csci3081::Drone* drone, const picojson::object& details,
                   csci3081::RechargeDrone* recharge_drone) {
  package_ = package;
  customer_ = customer;
  drone_ = drone;
  recharge_drone_ = recharge_drone;
  details_ = details;
  SetDestination();
}

void Delivery::SetDestination() {
  if (!drone_->HasPackage()) {
    drone_->SetDestination(package_->GetPosition());
    drone_->SetRoute();
  } else {
    drone_->SetDestination(customer_->GetPosition());
    drone_->SetRoute();
  }
}

void Delivery::Update(float dt) {
  if (drone_->GetRemainingBattery() <= 0) {
    if (drone_->GetBackupBattery() > 0) {
      drone_->SetBackupBattery(dt * 20);
    } else {
      if (dead_backup_battery_ == false) {
        // This breaks the pathing so I commented it out - Adam
        // drone_->SetYPosition(251);
        // recharge_drone_->SetDestination(drone_->GetPosition());
        // recharge_drone_->SetRoute();
        dead_backup_battery_ = true;
      }
    }
    // if drone just died, send an idle notification
    if (!drone_->idle_) {
      drone_->idle_ = true;
      drone_->SendNotifications("idle");
      // Change color to red
      drone_->SendNotifications("updateDetails");
    }
    // set available recharge drone path to dead battery
    if (recharge_drone_->out_for_delivery_ == 0) {
      recharge_drone_->SetDestination(drone_->GetPosition());
      recharge_drone_->SetRoute();
      recharge_drone_->out_for_delivery_ = 1;
    }
    recharge_drone_->Update(dt);
    if (WithinRange(recharge_drone_, drone_)) {
      drone_->RefillBattery();
      // notification drone is back en route
      drone_->idle_ = false;
      drone_->SendNotifications("moving");
      // change color back to green
      drone_->SendNotifications("updateDetails");
      // recharge drone set next destination to homebase
      recharge_drone_->SetDestination(recharge_drone_->GetHomeBase());
      recharge_drone_->SetRoute();
    }
  } else {
    dead_backup_battery_ = false;

    // move recharge drone back to homeBase
    if ((recharge_drone_->out_for_delivery_ == 1) &&
        ((recharge_drone_->GetHomeBase()[0] !=
          recharge_drone_->GetPosition()[0]) ||
         (recharge_drone_->GetHomeBase()[1] !=
          recharge_drone_->GetPosition()[1]) ||
         (recharge_drone_->GetHomeBase()[2] !=
          recharge_drone_->GetPosition()[2]))) {
      recharge_drone_->Update(dt);
    }

    if (!package_->GetReceived()) {
      drone_->Update(dt);
      customer_->Update(dt);
      if (drone_->HasPackage()) {
        package_->Update(dt, drone_->GetPosition());
      }
      // If the drone is within range of the package, pick it up and set a new
      // course for the customer.
      if (WithinRange(drone_, package_)) {
        if (!drone_->HasPackage()) {
          drone_->PickUp();
          drone_->AddWeightToLoad(package_->GetWeight());
          package_->SetDynamic(true);
          package_->SendNotifications("en route");
          drone_->SetDestination(customer_->GetPosition());
          drone_->SetRoute();
        }
      }
      // if the drone is within range of the customer and it has a package, drop
      // it off to the customer.
      if (WithinRange(drone_, customer_) && drone_->HasPackage()) {
        drone_->SendNotifications("idle");
        drone_->DropOff();
        drone_->SubtractWeightFromLoad(package_->GetWeight());
        package_->SetReceived();
        package_->SendNotifications("delivered");
        package_->SetDynamic(false);
        drone_->out_for_delivery_ = false;
      }
    }
  }
}

/**
 * WithinRange takes a Drone and Entity to see if the Drone has reached
 * either a Package to be picked up, or a customer to complete the delivery.
 * The Drone will then pick up the package or drop off the package.
 */
bool Delivery::WithinRange(entity_project::Entity* source,
                           entity_project::Entity* destination) {
  Vec3 drone_pos = Vec3(source->GetPosition());
  Vec3 dest_pos = Vec3(destination->GetPosition());
  Vec3 distance = dest_pos - drone_pos;
  if (distance.Length() <= source->GetRadius() + destination->GetRadius()) {
    return true;
  }
  return false;
}
