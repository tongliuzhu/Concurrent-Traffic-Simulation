#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Graphics.h"
#include "Intersection.h"

void Graphics::simulate()
{
    this->loadBackgroundImg();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // update graphics
        this->drawTrafficObjects();
    }
}

void Graphics::loadBackgroundImg()
{
    // create window
    _windowName = "Concurrency Traffic Simulation";
    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(_bgFilename);
    _images.push_back(background);         // first element is the original background
    _images.push_back(background.clone()); // second element will be the transparent overlay
}

void Graphics::drawTrafficObjects()
{
    // reset images
    _images.at(1) = _images.at(0).clone();

    // create overlay from all traffic objects
    for (auto it : _trafficObjects)
    {
        double posx, posy;
        it->getPosition(posx, posy);

        if (it->getType() == ObjectType::objectIntersection)
        {
            // cast object type from TrafficObject to Intersection
            std::shared_ptr<Intersection> intersection = std::dynamic_pointer_cast<Intersection>(it);

            // set color according to traffic light and draw the intersection as a circle
            TrafficLightPhase color;
            cv::Scalar trafficLightColor;
            intersection->trafficLightIsGreen(color); // get color
            if (color == TrafficLightPhase::red)      //red
            {
                trafficLightColor = cv::Scalar(0, 0, 255);
            }
            else if (color == TrafficLightPhase::yellow)
            {
                trafficLightColor = cv::Scalar(0, 255, 255);
            }
            else
            {
                trafficLightColor = cv::Scalar(0, 255, 0);
            }
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 25, trafficLightColor, -1);
        }
        else if (it->getType() == ObjectType::objectVehicle)
        {
            cv::RNG rng(it->getID());
            int b = rng.uniform(0, 255);
            int g = rng.uniform(0, 255);
            int r = sqrt(255 * 255 - g * g - r * r); // ensure that length of color vector is always 255
            cv::Scalar vehicleColor = cv::Scalar(b, g, r);
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 50, vehicleColor, -1);
        }
    }
    // display background and overlay image
    cv::imshow(_windowName, _images.at(1));
    cv::waitKey(33);
}
