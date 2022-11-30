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
    //ros::Subscriber location_updated_sub = nh.subscribe((ThisNamespace+"/location_updated").c_str(), 1, locationUpdatedCallback);
    ros::Subscriber location_updated_sub;

    // define TA states as enum
    enum STATES 
    {
        Start, CheckMembers, ElectMembers, UpdateMembers, LeaderElection, MissionStarted, MissionAccomplished
    } STATE;

    STATE = Start;

    //fixing re declaration errors
    int updating_mission = true; 
    int vote_counter = 0;       
    int Needed = 3;
    vector<int> goal;

    //define standard msg
    std_msgs::Int8 sync;
    sync.data = 1;

    ROS_INFO("Mission Control going into while loop");
    ros::Duration(5).sleep(); // sleep for 5 second

    // rate of 1 Hz  
    //frequency that you would like to loop at. It will keep track of how long it has been since the last call to Rate::sleep(), and sleep for the correct amount of time.
    ros::Rate rate(1.0);
    // consider increasing freq to 10 Hz
    while(ros::ok()){

        //ROS_INFO("ROS IS OK");

        ros::spinOnce();
        //ros::Duration(5).sleep(); // sleep for half a second
        

        switch(STATE){

            case Start:
            {
                ROS_INFO("MissionControl in Start state");
                nh.getParam("/goal", goal);
                nh.setParam("/goal_location", goal);     // do it exactly like uppaal, element by element modify for future 
                STATE = CheckMembers;
                //ros::Duration(3).sleep(); // sleep for half a second
                break;
            }    
            case CheckMembers:
            {    
                //ROS_INFO("Mission Control in Check Members state");     
                nh.getParam("/updating_mission", updating_mission);
                //ROS_INFO("updating_mission: %d", updating_mission);
                if(updating_mission == true){
                    
                    if(update_status_pub.getNumSubscribers() < 3){                         
                        ROS_INFO("waiting for udpdate status sub");
                    }else{
                        update_status_pub.publish(sync);
                        MissionController->possible_member();
                        STATE = ElectMembers;
                    }
                }
                rate.sleep();
                break;
            }

            case ElectMembers:
            {    
                ROS_INFO("Mission Control in ElectMembers state"); 
                nh.getParam("/updating_mission", updating_mission);
                ROS_INFO("updating mission should be false %d", updating_mission); 
                
                if(updating_mission == true){
                    
                    if(member_election_pub.getNumSubscribers() < 3){
                        ROS_INFO("waiting for updating mission sub elect members state");
                    }else{
                        nh.setParam("/updating_mission", false);
                        member_election_pub.publish(sync);
                    }
                }
                nh.getParam("/updating_mission", updating_mission);
                if(updating_mission == false){
                    MissionController->elect_members();
                    ROS_INFO("Mission Control selected members");
                    STATE = UpdateMembers;
                }

                break;
            }

            case UpdateMembers: 
            {
                if(update_status_pub.getNumSubscribers() < 3){
                    ROS_INFO("waiting for upate status sub update members");
                }else{
                    update_status_pub.publish(sync);
                    updating_mission = true;
                    nh.setParam("/updating_mission", updating_mission);
                }
                STATE = LeaderElection;
                break;     
            }

            case LeaderElection:
            {
                nh.getParam("/vote_counter", vote_counter);       // vote counter increasing indefinitely 
                nh.getParam("/updating_mission", updating_mission);
                
                if(vote_counter == 0 && updating_mission == true){
                    
                    if(election_pub.getNumSubscribers() < 3){
                        ROS_INFO("waiting for election subs leader election state");
                    }else{
                        election_pub.publish(sync);
                        updating_mission = false;
                        nh.setParam("/updating_mission", updating_mission);
                    }
                }
                nh.getParam("/Needed", Needed);
                if(vote_counter == Needed){
                    
                    if(update_status_pub.getNumSubscribers() < 3){
                        ROS_INFO("waiting for update status subs leader election state");
                    }else{
                        update_status_pub.publish(sync);
                        MissionController->elect_leader();
                        STATE = MissionStarted;
                    }
                    
                }

                break;
            }

            case MissionStarted:
            {
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
               
                if(MissionController->swarm_reached_goal()){
                    if(mission_end_pub.getNumSubscribers() < 3){
                        ROS_INFO("waiting for mission end subs mission started state");
                    }else{
                        mission_end_pub.publish(sync);
                        nh.setParam("/updating_mission", 0);   
                        STATE = MissionAccomplished;
                    }
                    
                }            
                
                location_updated_sub = nh.subscribe((ThisNamespace+"/location_updated").c_str(), 1, locationUpdatedCallback);
                if(location_updated_var == 1){                  
                    nh.setParam("/updating_mission", true);
                    nh.setParam("/vote_counter", 0);
                    MissionController->reset_arrays();
                    STATE = CheckMembers;
                    location_updated_sub.shutdown();
                }
                
                break;
            }

            case MissionAccomplished:
            
                break;

        }

        rate.sleep();

    }

    ROS_INFO("ROS IS NOT OK");

}
