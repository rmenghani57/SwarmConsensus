#include "base_classes/Drone_base_class.cpp"
#include "ros/ros.h"
#include <thread>
#include <chrono>
using namespace std;

// locally global variables to use in callback functions
int update_status_var;
int member_election_var;
int leader_election_var;
int update_location_var;
int mission_end_var;

// same as mission control template
int location_updated_var;

void locationUpdatedCallback(int location_updated){

    ROS_INFO("location_updated = [%d]", location_updated);
    location_updated_var = location_updated;

}


void statusCallback(int update_status){

    ROS_INFO("update_stautus = [%d]", update_status);
    update_status_var = update_status;

}

void memberElectionCallback(int member_election){
    ROS_INFO("member_election = [%d]", member_election);
    member_election_var = member_election;
}

void leaderElectionCallback(int leader_election){
    ROS_INFO("leader_election = [%d]", leader_election);
    leader_election_var = leader_election;
}

void updateLocationCallback(int update_location){
    ROS_INFO("update_location = [%d]", update_location);
    update_location_var = update_location;
}

void missionEndCallback(int mission_end){
    ROS_INFO("update_location = [%d]", mission_end);
    mission_end_var = mission_end;
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
    ros::Subscriber update_status_sub = nh.subscribe("update_status", statusCallback);
    //whenever new message in topic member_election, statusCallback func is called
    ros::Subscriber member_election_sub = nh.subscribe("member_election", memberElectionCallback);

    ros::Subscriber leader_election_sub = nh.subscribe("leader_election", leaderElectionCallback);

    // both pub and sub for this topic in same node, could cause problems
    ros::Subscriber update_location_sub = nh.subscribe("update_location", updateLocationCallback;


    // same as mission control template, might cause problems
    ros::Subscriber location_updated_sub = nh.subscribe("location_updated", locationUpdatedCallback);

    //mission end subscirber
    ros::Subscriber mission_end_sub = nh.subscribe("mission_end", missionEndCallback);


    //Publishers
    ros::Publisher location_updated_pub = nh.advertise<int>("location_updated", 1);

    // publisher and subscriber in same node (here)
    ros::Publisher update_location_pub = nh.advertise<int>("update_location", 1);

    // rate of 1 Hz
    //frequency that you would like to loop at. It will keep track of how long it has been since the last call to Rate::sleep(), and sleep for the correct amount of time.
    ros::Rate rate(1.0);

    // parse the argument passed in launch file to represent current drone id
    int id = atoi(argv[1]);

    // instantiate Drone class
    Drone* ThisDrone = new Drone(id);

    // local global variable to get and set vote counter variable
    int vote_counter;

    int Needed;
    nh.getParam("/Needed", Needed);

    int updating_mission;

    // define TA states as enum
    enum STATES 
    {
        Idle, InSwarm, Leader, UpdatingLocation, WaitingSwarm    //check if this is correct or need strings

    } STATE;

    STATE = Idle;

    while(ros::ok()){

        ros::spinOnce();

        switch(STATE){

            case Idle:
                // idle to in swarm
                if(update_status_var == 1 && ThisDrone->in_swarm(id)){
                    STATE = InSwarm;
                }
                break;     

            case InSwarm:
                // member election logic
                if(member_election_var == 1 && ThisDrone->in_swarm(id)){
                    ThisDrone->vote_member(id);
                    nh.getParam("/vote_counter", vote_counter);
                    vote_counter += 1;
                    nh.setParam("/vote_counter", vote_counter); 
                }
                //leader election logic
                if(leader_election_var == 1 && ThisDrone->in_swarm(id)){
                    ThisDrone->vote(id);
                }
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
                //inSwarm to leader transition
                if(ThisDrone->is_leader(id) && vote_counter == Needed && updating_mission == 1){
                    ThisDrone->update_leader_position(id);
                    STATE = Leader;
                }
                // InSwarm to UpdatingLocation state
                if(ThisDrone->in_swarm(id) && ThisDrone->reached_goal(id) == false && update_location_var == 1){
                    ThisDrone->move(id);
                    STATE = UpdatingLocation;
                }
                // InSwarm to Idle transition
                if(ThisDrone->in_swarm(id) == false && update_status_var == 1){
                    STATUS = Idle;
                }

                break;

            case Leader:
                if(ThisDrone->is_leader(id) == false){
                    STATE = InSwarm;
                }
                if(mission_end_var == 1){
                    STATE = Idle;
                }
                nh.getParam("/updating_mission", updating_mission);
                if(ThisDrone->swarm_reached_goal() == false && ThisDrone->is_leader(id) && updating_mission == true){
                    update_location_pub.publish(1);
                    ThisDrone->move(id);
                    STATE = WaitingSwarm;
                }

                break;

            case UpdatingLocation:
                if(location_updated_var == 1){
                    STATE = InSwarm;
                }
                break;  

            case WaitingSwarm:
                nh.getParam("/vote_counter", vote_counter);
                if(vote_counter == 0){
                    location_updated_pub.publish(1);
                    STATE = InSwarm;
                }
                break;

        }

    }
    














}
