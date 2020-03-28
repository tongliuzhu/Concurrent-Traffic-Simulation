#include <iostream>
#include <random>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.clear();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::unique_lock<std::mutex> lck(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight(int id)
{
    _currentPhase = TrafficLightPhase::red;
    _id = id;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto msg = _trafficQueue.receive(); //_currentPhase; //
        if (TrafficLightPhase::green == msg)
        {
            std::cout << "Light ID: " << getId() << " has turn to green\n";
            break;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
    //threads.emplace_back(std::thread(&Intersection::processVehicleQueue, get_shared_this()));
    //FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called.
    //To do this, use the thread queue in the base class.
}

//virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles
    // and toggles the current phase of the traffic light between red and green and sends an update method
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds.
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(3, 6);
    double cycleDuration = 1;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        if (timeSinceLastUpdate >= cycleDuration)
        {
            // take a duration by 4-6 seconds.
            std::this_thread::sleep_for(std::chrono::seconds(distr(eng)));
            if (_currentPhase == TrafficLightPhase::green)
            {
                std::lock_guard<std::mutex> lck(_mutex);
                _currentPhase = TrafficLightPhase::yellow;
            }
            else if (_currentPhase == TrafficLightPhase::yellow)
            {
                std::lock_guard<std::mutex> lck(_mutex);
                _currentPhase = TrafficLightPhase::red;
            }
            else
            {
                std::lock_guard<std::mutex> lck(_mutex);
                _currentPhase = TrafficLightPhase::green;
            }
            auto temp_phase = _currentPhase;
            auto sentFuture = std::async(std::launch::async,
                                         &MessageQueue<TrafficLightPhase>::send,
                                         &_trafficQueue,
                                         std::move(temp_phase));
            sentFuture.wait();
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}