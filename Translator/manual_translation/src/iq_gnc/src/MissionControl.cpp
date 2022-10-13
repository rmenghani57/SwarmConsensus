#include "base_classes/MissionControl_base_class.cpp"
#include <thread>
#include <chrono>
using namespace std;

int main(int argc, char** argv)
{

    // initialize ROS
    ros::init(argc, argv, "MissionControl_node");
    // private node handle
    ros::NodeHandle nh("~");

    MissionControl* MissionController = new MissionControl();

    ros::Rate rate(1.0);

    while(ros::ok()){

        ros::spinOnce();

        
        








    }




}
