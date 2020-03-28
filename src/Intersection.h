#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include <future>
#include <mutex>
#include <memory>
#include "TrafficObject.h"
#include "TrafficLight.h"

// forward declarations to avoid include cycle
class Street;
class Vehicle;

// auxiliary class to queue and dequeue waiting vehicles in a thread-safe manner
class WaitingVehicles
{
public:
    // getters / setters
    int getSize();

    // typical behaviour methods
    void pushBack(std::shared_ptr<Vehicle> vehicle, std::promise<void> &&promise);
    void permitEntryToFirstInQueue();

private:
    std::vector<std::shared_ptr<Vehicle>> _vehicles;          // list of all vehicles waiting to enter this intersection
    std::vector<std::promise<void>> _promises; // list of associated promises
    std::mutex _mutex;
};

class Intersection : public TrafficObject, public std::enable_shared_from_this<Intersection>
{
public:
    // constructor / desctructor
    Intersection(int id);

    // getters / setters
    void setIsBlocked(bool isBlocked);
    inline int getIntersectionId() {return _intersectionId;};

    // typical behaviour methods
    void addVehicleToQueue(std::shared_ptr<Vehicle> vehicle);
    void addStreet(std::shared_ptr<Street> street);
    std::vector<std::shared_ptr<Street>> queryStreets(std::shared_ptr<Street> incoming); // return pointer to current list of all outgoing streets
    void simulate();
    void vehicleHasLeft(std::shared_ptr<Vehicle> vehicle);
    bool trafficLightIsGreen(TrafficLightPhase& color);

    // miscellaneous
    std::shared_ptr<Intersection> get_shared_this() { return shared_from_this(); }

private:

    // typical behaviour methods
    void processVehicleQueue();

    // private members
    std::vector<std::shared_ptr<Street>> _streets;   // list of all streets connected to this intersection
    WaitingVehicles _waitingVehicles; // list of all vehicles and their associated promises waiting to enter the intersection
    bool _isBlocked;                  // flag indicating wether the intersection is blocked by a vehicle
    TrafficLight _trafficLight;
    int _intersectionId;
};

#endif
