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

    init_publisher_subscriber(nh);

    // this node publishes to the update_status topic which is an Uppaal channel (triggered in MissionControl Template)
    ros::Publisher update_status_pub = nh.advertise<int>("update_status", 1);

    // rate of 1 Hz
    //frequency that you would like to loop at. It will keep track of how long it has been since the last call to Rate::sleep(), and sleep for the correct amount of time.
    ros::Rate rate(1.0);


    // define TA states as enum
    enum STATES 
    {
        Start, CheckMembers, ElectMembers, UpdateMembers, LeaderElection, MissionStarted, MissionAccomplished
    } STATE;

    STATE = Start;

    while(ros::ok()){

        ros::spinOnce();

        switch(STATE){

            case Start:
                nh.setParam("/goal_location", nh.getParam(goal);
                STATE = CheckMembers;
                break;
                
            case CheckMembers:
                if(nh.getParam(updating_mission) == true){
                    MissionController->possible_member();

                }
























        }
        








    }




}
