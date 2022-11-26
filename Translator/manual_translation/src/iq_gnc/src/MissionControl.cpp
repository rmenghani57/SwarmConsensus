#include "base_classes/MissionControl_base_class.cpp"
#include "ros/ros.h"
#include <thread>
#include <chrono>
#include <std_msgs/Int8.h>

using namespace std;

int location_updated_var;

void locationUpdatedCallback(std_msgs::Int8 location_updated){

    ROS_INFO("location_updated = [%d]", location_updated.data);
    location_updated_var = location_updated.data;

}


int main(int argc, char** argv)
{

    // initialize ROS
    ros::init(argc, argv, "MissionControl_node");
    // private node handle
    ros::NodeHandle nh("~");

    init_publisher_subscriber(nh);

    //create local reference frame 
	//initialize_local_frame();

    MissionControl* MissionController = new MissionControl();


    // all publishers - each represents a channel in uppaal
    // this node publishes to the update_status topic which is an Uppaal channel (triggered in MissionControl Template)
    ros::Publisher update_status_pub = nh.advertise<std_msgs::Int8>("update_status", 1);
    ros::Publisher member_election_pub = nh.advertise<std_msgs::Int8>("member_election", 1);
    ros::Publisher election_pub = nh.advertise<std_msgs::Int8>("leader_election", 1);
    ros::Publisher mission_end_pub = nh.advertise<std_msgs::Int8>("mission_end", 1);

    //subscribers
    ros::Subscriber location_updated_sub = nh.subscribe("location_updated", 1, locationUpdatedCallback);

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

    //define standard msg
    std_msgs::Int8 sync;
    sync.data = 1;

    while(ros::ok()){

        ros::spinOnce();

        switch(STATE){

            case Start:
                int goal;
                nh.getParam("/goal", goal);
                nh.setParam("/goal_location", goal);     // do it exactly like uppaal, element by element modify for future 
                STATE = CheckMembers;
                break;
                
            case CheckMembers:
                      
                nh.getParam("/updating_mission", updating_mission);
                if(updating_mission == true){
                    MissionController->possible_member();
                    update_status_pub.publish(sync);
                    STATE = ElectMembers;
                }
                break;

            case ElectMembers:
                     
                nh.getParam("/updating_mission", updating_mission);
                if(updating_mission == true){
                    nh.setParam("/updating_mission", false);
                    member_election_pub.publish(sync);
                }
                if(updating_mission == false){
                    MissionController->elect_members();
                    STATE = UpdateMembers;
                }
                break;

            case UpdateMembers:
                nh.setParam("/updating_mission", true);
                update_status_pub.publish(sync);
                STATE = LeaderElection;
                break;     

            case LeaderElection:
                nh.getParam("/vote_counter", vote_counter);
                     
                nh.getParam("/updating_mission", updating_mission);
                if(vote_counter == 0 && updating_mission == true){
                    updating_mission = false;
                    nh.setParam("/updating_mission", false);
                    election_pub.publish(sync);
                }
                else{
                    int Needed;
                    nh.getParam("/Needed", Needed);
                    if(vote_counter == Needed){
                        MissionController->elect_leader();
                        update_status_pub.publish(sync);
                        STATE = MissionStarted;
                    }                      

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
                else{
                    // callback function does not return
                    if(location_updated_var == 1){
                        nh.setParam("/updating_mission", true);
                        nh.setParam("/vote_counter", 0);
                        MissionController->reset_arrays();
                        STATE = CheckMembers;
                    }
                }
                break;

            case MissionAccomplished:
            
                break;

        }
    }
}
