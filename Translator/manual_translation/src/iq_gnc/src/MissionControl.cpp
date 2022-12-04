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
    int updating_mission; 
    int vote_counter;       
    int Needed;
    vector<int> goal;

    //define standard msg
    std_msgs::Int8 sync;
    sync.data = 1;


    ros::Duration(10).sleep();

    ROS_INFO("Mission Control going into while loop");
    
    ros::Rate rate(0.5);
    
    while(ros::ok()){

        ros::spinOnce();

        switch(STATE){

            case Start:
            {   
                // start to check members transition
                ROS_INFO("MissionControl in Start state");
                nh.getParam("/goal", goal);
                nh.setParam("/goal_location", goal); 
                STATE = CheckMembers;
                rate.sleep();
                break;
            }    

            case CheckMembers:
            {    
                // check members to elect members transition
                ROS_INFO("Mission Control in Check Members state");     
                nh.getParam("/updating_mission", updating_mission);
                if(updating_mission == 1){
                    
                    update_status_pub.publish(sync); // update status can have 0 subs
                    MissionController->possible_member();
                    STATE = ElectMembers;
                    rate.sleep();
                    break;
                  
                }

                break;
            }

            case ElectMembers:
            {    
                // elect members self loop
                nh.getParam("/updating_mission", updating_mission);                
                if(updating_mission == 1){
                    
                    while(member_election_pub.getNumSubscribers() < 3){
                        //ROS_INFO("waiting for updating mission sub elect members state");
                    }
                    
                    ROS_INFO("Setting updating mission false");
                    nh.setParam("/updating_mission", 0);
                    member_election_pub.publish(sync);
                    STATE = ElectMembers;
                    rate.sleep();                 
                }
                
                //trying to stop more than 3 voting loops
                nh.getParam("/vote_counter", vote_counter);
                ROS_INFO("Number of votes in parameter server: %d", vote_counter);
                while(vote_counter < 3){
                    nh.getParam("/vote_counter", vote_counter);
                }

                ROS_INFO("updating mission should be false %d", updating_mission);
                // electmembers to update members transition
                ROS_INFO("Mission Control in ElectMembers state"); 
                nh.getParam("/updating_mission", updating_mission);
                if(updating_mission == 0){
                    MissionController->elect_members();
                    nh.getParam("/vote_counter", vote_counter);
                    ROS_INFO("After MC member election, vote counter should be 0: %d", vote_counter);
                    STATE = UpdateMembers;
                    rate.sleep();
                    break;
                }       

                rate.sleep();
                break;
            }

            case UpdateMembers: 
            {
                // updatemembers to leader election transition
                ROS_INFO("MC in UpdateMembers, publishing update status. ");  // GOT HERE
                
                update_status_pub.publish(sync);   //update status can have 0 subs
                nh.setParam("/updating_mission", 1);
                STATE = LeaderElection;
                rate.sleep();                    
                break;     
            }

            case LeaderElection:
            {
                // leader election self loop
                nh.getParam("/vote_counter", vote_counter);       
                nh.getParam("/updating_mission", updating_mission);
                if(vote_counter == 0 && updating_mission == 1){ 
                    while(election_pub.getNumSubscribers() < 3){
                        ROS_INFO("waiting for election subs leader election state");
                    }
                    election_pub.publish(sync); 
                    ROS_INFO("Mission Control Sending Leader Election sync");
                    nh.setParam("/updating_mission", 1);
                    rate.sleep();                
                }

                nh.getParam("/vote_counter", vote_counter); 
                while(vote_counter < 3){
                    // wait till votes are 3
                    nh.getParam("/vote_counter", vote_counter);
                }

                // leaderelection to missionstarted transition
                nh.getParam("/Needed", Needed);
                if(vote_counter == Needed){
                    update_status_pub.publish(sync);
                    MissionController->elect_leader();
                    ROS_INFO("Mission Control Leader elected");
                    STATE = MissionStarted;
                    rate.sleep();
                    break;
                    
                }
                rate.sleep();
                break;
            }

            case MissionStarted:
            {
                // missionstarted to missionaccomplished transition
                ROS_INFO("MC in mission started state");
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
                if(MissionController->swarm_reached_goal()){
                    while(mission_end_pub.getNumSubscribers() < 3){
                        ROS_INFO("waiting for mission end subs mission started state");
                    }
                  
                    mission_end_pub.publish(sync);
                    nh.setParam("/updating_mission", 0);   
                    STATE = MissionAccomplished;
                    rate.sleep();
                    break;
                }            
                
                // missionstarted to checkmembers transition
                location_updated_sub = nh.subscribe("/location_updated", 1, locationUpdatedCallback);
                while(location_updated_var != 1){
                    ros::spinOnce();
                }
                if(location_updated_var == 1){           
                    nh.setParam("/updating_mission", 1);
                    nh.setParam("/vote_counter", 0);
                    MissionController->reset_arrays();
                    STATE = CheckMembers;
                    location_updated_sub.shutdown();
                    location_updated_var = 0;
                    rate.sleep();
                    break;
                }
                rate.sleep();
                break;
            }

            case MissionAccomplished:
            
                break;

        }

        rate.sleep();

    }

    ROS_INFO("ROS IS NOT OK");

}
