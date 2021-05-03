/*! \mainpage Project ANVIL Design
 *
 * \section contributors Contributors
 * - Eileen Campbell (camp0870)
 * - Adam Comerford (comer059)
 * - Catherine Lewis (lewi1108)
 * - Claire Li (li000494)
 *
 * \section intro_sec Description
 *
 * This page documents the Drone Delivery System subsystem. This subsystem
 * encapsulates the Drone Delivery System Facade, Entity Factory, Delivery
 * scheduling/handling, and the Entity Observers. It also contains
 * the Drone, Package, and Customer Entity interfaces.
 *
 * \section section2 Getting Started
 *
 * The Drone Delivery System Subsystem is responsible for Scheduling deliveries,
 * which consist of Packages, Customers, and Drones, and simulating the Drone
 * navigating to a Package, picking it up, then navigating to the Customer to
 * drop it off. Here are the required classes for this functionality:
 *
 * **You will find the following classes:** \n
 * csci3081::Customer - customer entity  \n
 * csci3081::Drone - drone entity  \n
 * csci3081::RechargeDrone - entity to recharge other Drone batteries \n
 * csci3081::DroneSimulation - Implementation of DroneDeliverySystem facade  \n
 * csci3081::JsonHelper - functions for Json operations  \n
 * csci3081::Package - package entity  \n
 * Delivery - Handles entities on a delivery \n
 * Scheduler - Schedules deliveries  \n
 * EntityFactory - creates entities  \n
 * EntityBase - an entity base class which does update/initialization.
 * This is not implemented as of now but is here for planning purposes for
 * future iterations. As of now, all these methods are implemented in the
 * Drone, Package, and Customer classes.  \n
 * Vec3 - a 3D vector library used for update calculations.
 *
 * \subsection subsection1 Iteration 3 Enhancement
 * Our team chose to implement recharging functionality in the form of a
 * RechargeDrone class. When a delivery Drone runs out of battery, a RechargeDrone
 * is sent to its location to deliver a battery refill. The delivery Drone then
 * continues along its route to the customer, while the RechargeDrone returns to a
 * station, which we have chosen to be in front of the student union. In addition,
 * the Drone now has a backup battery which provides enough power to remain in the
 * air after the main battery has died. If this backup battery dies as well, the Drone
 * drops to the ground. We have also implemented some simple color features to add clarity
 * to the simulation.
 * 
 * \subsection subsection2 Basic Flow:
 * The basic flow of events is as follows:
 * - Call CreateEntity to create a new entity object.
 * - Call AddEntity to add this to the list of entity objects.
 * - Call ScheduleDelivery to assign a Package, Customer and Drone to a Delivery
 * object.
 * - Use the Delivery object for updating the situation of the delivery:
 *   - Set destination of Drone to the Package.
 *   - Update the position of a Drone
 *   - Check if it's within range of a package.
 *   - Pick up package.
 *   - Set Destination of Drone to Customer.
 *   - Update position of Drone & Package.
 *   - Check if Drone is within range of Customer.
 *   - Deliver Package to Customer.
 *   - If a Drone runs out of battery while on a delivery, it becomes idle. 
 * A RechargeDrone is then dispatched to refill the battery.
 *
 * \section section3 Build and Run
 * First build the Docker Image environment:
 *      \code ./bin/build-env.sh \endcode
 * Then run the environment:
 *      \code ./bin/run-env.sh \endcode
 * Now navigate to the project directory, make and run the simulation:
 *      \code cd project
 *      make
 *      ./bin/run.sh \endcode
 * Type in the server port to a web browser to view the simulation:
 *      \code http://127.0.0.1:8081/ \endcode
 * You should now see the simulation running on the web server.
 *
 * \subsection subsection3 UML Diagram:
 * \image html UML.jpeg "UML diagram:"
 *
 */
