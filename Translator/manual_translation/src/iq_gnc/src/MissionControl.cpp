#include "base_classes/MissionControl_base_class.cpp"
#include "ros/ros.h"
#include <thread>
#include <chrono>
#include <std_msgs/Int8.h>

using namespace std;

int location_updated_var;

void locationUpdatedCallback(std_msgs::Int8 location_updated){

    //ROS_INFO("location_updated = [%d]", location_updated.data);
    location_updated_var = location_updated.data;

}


int main(int argc, char** argv)
{

    // initialize ROS
    ros::init(argc, argv, "MissionControl_node");
    // private node handle
    ros::NodeHandle nh("~");

    init_publisher_subscriber(nh);


    MissionControl* MissionController = new MissionControl();

    std::string ThisNamespace;
    nh.getParam("namespace", ThisNamespace);
    ROS_INFO("THIS NAMESPACE IS: %s", ThisNamespace.c_str());

    // all publishers - each represents a channel in uppaal
    // this node publishes to the update_status topic which is an Uppaal channel (triggered in MissionControl Template)
    ros::Publisher update_status_pub = nh.advertise<std_msgs::Int8>((ThisNamespace+"/update_status").c_str(), 1, true);
    ros::Publisher member_election_pub = nh.advertise<std_msgs::Int8>((ThisNamespace+"/member_election").c_str(), 1, true);
    ros::Publisher election_pub = nh.advertise<std_msgs::Int8>((ThisNamespace+"/leader_election").c_str(), 1, true);
    ros::Publisher mission_end_pub = nh.advertise<std_msgs::Int8>((ThisNamespace+"/mission_end").c_str(), 1, true);

    //subscribers
    ros::Subscriber location_updated_sub = nh.subscribe((ThisNamespace+"/location_updated").c_str(), 1, locationUpdatedCallback);

    // rate of 1 Hz  
    //frequency that you would like to loop at. It will keep track of how long it has been since the last call to Rate::sleep(), and sleep for the correct amount of time.
    ros::Rate rate(2.0);
    // consider increasing freq to 10 Hz


    // define TA states as enum
    enum STATES 
    {
        Start, CheckMembers, ElectMembers, UpdateMembers, LeaderElection, MissionStarted, MissionAccomplished
    } STATE;

    STATE = Start;

    //fixing re declaration errors
    int updating_mission; 
    int vote_counter;       
    int Needed;

    //define standard msg
    std_msgs::Int8 sync;
    sync.data = 1;

    ROS_INFO("Mission Control going into while loop");


    while(ros::ok()){

        //ROS_INFO("ROS IS OK");

        ros::spinOnce();

        switch(STATE){


            case Start:
                ROS_INFO("MissionControl in Start state");
                int goal;
                nh.getParam("/goal", goal);
                nh.setParam("/goal_location", goal);     // do it exactly like uppaal, element by element modify for future 
                ROS_INFO("Printing goal location: %d", goal);
                STATE = CheckMembers;
                break;
                
            case CheckMembers:
                //ros::Duration(10).sleep(); // sleep for 10 seconds
                //ROS_INFO("Mission Control in Check Members state");     
                nh.getParam("/updating_mission", updating_mission);
                //ROS_INFO("updating_mission: %d", updating_mission);
                if(updating_mission == true){
                    ROS_INFO("MissionControl publishing update status");
                    MissionController->possible_member();
                    update_status_pub.publish(sync);
                    STATE = ElectMembers;
                }
                break;

            case ElectMembers:
                //ROS_INFO("Mission Control in ElectMembers state");  REACHES HERE
                nh.getParam("/updating_mission", updating_mission);
                if(updating_mission == true){
                    nh.setParam("/updating_mission", false);
                    member_election_pub.publish(sync);
                }
                else if(updating_mission == false){
                    MissionController->elect_members();
                    STATE = UpdateMembers;
                }
                break;

            case UpdateMembers:
                updating_mission = true;
                nh.setParam("/updating_mission", updating_mission);
                update_status_pub.publish(sync);
                STATE = LeaderElection;
                break;     

            case LeaderElection:
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
                nh.getParam("/Needed", Needed);

                if(vote_counter == 0 && updating_mission == true){
                    updating_mission = false;
                    nh.setParam("/updating_mission", updating_mission);
                    election_pub.publish(sync);
                }
                else if(vote_counter == Needed){
                    MissionController->elect_leader();
                    update_status_pub.publish(sync);
                    STATE = MissionStarted;
                }else{
                    STATE = LeaderElection;
                }                      
                break;

            case MissionStarted:
            
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
               
                if(MissionController->swarm_reached_goal()){
                    mission_end_pub.publish(sync);
                    nh.setParam("/updating_mission", 0);   
                    STATE = MissionAccomplished;
                }            
                else if(location_updated_var == 1){                  // callback function does not return
                    nh.setParam("/updating_mission", true);
                    nh.setParam("/vote_counter", 0);
                    MissionController->reset_arrays();
                    STATE = CheckMembers;
                }else{
                    STATE = MissionStarted;
                }
                
                break;

            case MissionAccomplished:
            
                break;

        }
    }

    ROS_INFO("ROS IS NOT OK");

}
