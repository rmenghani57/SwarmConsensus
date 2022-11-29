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
                ROS_INFO("MissionControl in Start state");
                nh.getParam("/goal", goal);
                nh.setParam("/goal_location", goal);     // do it exactly like uppaal, element by element modify for future 
                STATE = CheckMembers;
                //ros::Duration(3).sleep(); // sleep for half a second
                break;
                
            case CheckMembers:
                //ros::Duration(10).sleep(); // sleep for 10 seconds
                //ROS_INFO("Mission Control in Check Members state");     
                nh.getParam("/updating_mission", updating_mission);
                //ROS_INFO("updating_mission: %d", updating_mission);
                if(updating_mission == true){
                    
                    if(update_status_pub.getNumSubscribers() < 1){                              ///ADDED!!!!!!!!!
                        ROS_INFO("waiting for udpdate status sub");
                    }else{
                        update_status_pub.publish(sync);
                        MissionController->possible_member();
                        STATE = ElectMembers;
                    }
                }
                break;

            case ElectMembers:
                ROS_INFO("Mission Control in ElectMembers state"); 
                nh.getParam("/updating_mission", updating_mission);
                ROS_INFO("updating mission should be false %d", updating_mission); 
                if(updating_mission == true){
                    nh.setParam("/updating_mission", false);
                    if(member_election_pub.getNumSubscribers() > 0){
                        member_election_pub.publish(sync);
                    }else{
                        rate.sleep();
                    }
                }
                else if(updating_mission == false){
                    MissionController->elect_members();
                    ROS_INFO("Mission Control selected members");
                    STATE = UpdateMembers;
                }else{
                    //STATE = ElectMembers;     // does this make sense? or just break
                    break;
                }
                break;

            case UpdateMembers: 
                updating_mission = true;
                nh.setParam("/updating_mission", updating_mission);
                if(update_status_pub.getNumSubscribers() > 0){
                    update_status_pub.publish(sync);
                }else{
                    rate.sleep();
                }
                STATE = LeaderElection;
                break;     

            case LeaderElection:
                nh.getParam("/vote_counter", vote_counter);       // vote counter increasing indefinitely 
                nh.getParam("/updating_mission", updating_mission);
                nh.getParam("/Needed", Needed);

                if(vote_counter == 0 && updating_mission == true){
                    updating_mission = false;
                    nh.setParam("/updating_mission", updating_mission);
                    if(election_pub.getNumSubscribers() > 0){
                        election_pub.publish(sync);
                    }{
                        rate.sleep();
                    }
                }
                else if(vote_counter == Needed){
                    MissionController->elect_leader();
                    if(update_status_pub.getNumSubscribers() > 0){
                        update_status_pub.publish(sync);
                    }else{
                        rate.sleep();
                    }
                    STATE = MissionStarted;
                }else{
                    //STATE = LeaderElection;
                    break;
                }                      
                break;

            case MissionStarted:
            
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
               
                if(MissionController->swarm_reached_goal()){
                    if(mission_end_pub.getNumSubscribers() > 0){
                        mission_end_pub.publish(sync);
                    }else{
                        rate.sleep();
                    }
                    nh.setParam("/updating_mission", 0);   
                    STATE = MissionAccomplished;
                }            
                else if(location_updated_var == 1){                  
                    nh.setParam("/updating_mission", true);
                    nh.setParam("/vote_counter", 0);
                    MissionController->reset_arrays();
                    STATE = CheckMembers;
                }else{
                    //STATE = MissionStarted;
                    break;
                }
                
                break;

            case MissionAccomplished:
            
                break;

        }

        rate.sleep();

    }

    ROS_INFO("ROS IS NOT OK");

}
