#include "base_classes/Drone_base_class.cpp"
#include "ros/ros.h"
#include <thread>
#include <chrono>
using namespace std;


void statusCallback(int msg){

    ROS_INFO("update_stautus = [%d]", msg);

}

int main(int argc, char** argv)
{

    // initialize ROS
    ros::init(argc, argv, "MissionControl_node");
    // private node handle
    ros::NodeHandle nh("~");

    // explanation on this?
    // This function is called at the beginning of a program and will start of the communication links to the FCU. The function requires the program's ros nodehandle as an input 
    // This function takes our ros node handle as an input and initializes subcribers that will collect the necessary information from our autopilot. 
    // @returns n/a
    init_publisher_subscriber(nh);

    //whenever new message in topic update_status, statusCallback func is called
    ros::Subscriber update_status_sub = nh.subscribe("update_status", 1, statusCallback);

    // do i instantiate 8 drone objects? but there are already 8 drone nodes in launch file, so this file will run 8 times in parallel?

    // rate of 1 Hz
    //frequency that you would like to loop at. It will keep track of how long it has been since the last call to Rate::sleep(), and sleep for the correct amount of time.
    ros::Rate rate(1.0);


    // define TA states as enum
    enum STATES 
    {
        Idle, InSwarm, Leader, UpdatingLocation. WaitingSwarm 
    } STATE;

    STATE = Idle;

    while(ros::ok()){

        ros::spinOnce();

        switch(STATE){

            case Idle:

                break;     

            case InSwarm:
                break;

            case Leader:
                break;

            case UpdatingLocation:
                break;  

            case WaitingSwarm:
                break;

        }


    }
    














}
