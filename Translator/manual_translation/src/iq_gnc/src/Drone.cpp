#include "base_classes/Drone_base_class.cpp"
#include "ros/ros.h"
#include <thread>
#include <chrono>
#include <std_msgs/Int8.h>

using namespace std;

// locally global variables to use in callback functions
int update_status_var;
int member_election_var;
int leader_election_var;
int update_location_var;
int mission_end_var;

// same as mission control template
int location_updated_var;

void locationUpdatedCallback(std_msgs::Int8 location_updated){

    ROS_INFO("location_updated = [%d]", location_updated.data);
    location_updated_var = location_updated.data;

}


void statusCallback(std_msgs::Int8 update_status){

    ROS_INFO("update_stautus = [%d]", update_status.data);
    update_status_var = update_status.data;

}

void memberElectionCallback(std_msgs::Int8 member_election){
    ROS_INFO("member_election = [%d]", member_election.data);
    member_election_var = member_election.data;
}

void leaderElectionCallback(std_msgs::Int8 leader_election){
    ROS_INFO("leader_election = [%d]", leader_election.data);
    leader_election_var = leader_election.data;
}

void updateLocationCallback(std_msgs::Int8 update_location){
    ROS_INFO("update_location = [%d]", update_location.data);
    update_location_var = update_location.data;
}

void missionEndCallback(std_msgs::Int8 mission_end){
    ROS_INFO("update_location = [%d]", mission_end.data);
    mission_end_var = mission_end.data;
}



int main(int argc, char** argv)
{

    // initialize ROS
    ros::init(argc, argv, "Drone_node");
    // private node handle
    ros::NodeHandle nh("~");

    // parse the argument passed in launch file to represent current drone id
    int id = atoi(argv[1]);

    // instantiate Drone class
    Drone* ThisDrone = new Drone(id);

    // explanation on this?
    // This function is called at the beginning of a program and will start of the communication links to the FCU. The function requires the program's ros nodehandle as an input 
    // This function takes our ros node handle as an input and initializes subcribers that will collect the necessary information from our autopilot. 
    // @returns n/a
    init_publisher_subscriber(nh);

    //ROS_INFO("In main, after pub sub init, before wait4connect");

    // wait for FCU connection
    //wait4connect();
    
    //ROS_INFO("Next Cmd is Set Guided Mode");

    // changing mode to GUIDED 
    //set_mode("GUIDED");

    //if(current_state_g.mode == "GUIDED")
	//{
	//	ROS_INFO("Mode set to GUIDED. Mission starting");
    //}

	//create local reference frame 
	initialize_local_frame();

    //try taking off all drones? in stabilize mode
    //testing
    //specify some waypoints 
	std::vector<gnc_api_waypoint> waypointList;
	gnc_api_waypoint nextWayPoint;
	nextWayPoint.x = 30;
	nextWayPoint.y = 30;
	nextWayPoint.z = 10;
	nextWayPoint.psi = 0;
	waypointList.push_back(nextWayPoint);
	nextWayPoint.x = 60;
	nextWayPoint.y = 60;
	nextWayPoint.z = 10;
	nextWayPoint.psi = 0;

    takeoff(10);
    

    //whenever new message in topic update_status, statusCallback func is called
    ros::Subscriber update_status_sub = nh.subscribe("update_status", 1, statusCallback);
    //whenever new message in topic member_election, statusCallback func is called
    ros::Subscriber member_election_sub = nh.subscribe("member_election", 1, memberElectionCallback);

    ros::Subscriber leader_election_sub = nh.subscribe("leader_election", 1, leaderElectionCallback);

    // both pub and sub for this topic in same node, could cause problems
    ros::Subscriber update_location_sub = nh.subscribe("update_location", 1, updateLocationCallback);


    // same as mission control template, might cause problems
    ros::Subscriber location_updated_sub = nh.subscribe("location_updated", 1, locationUpdatedCallback);

    //mission end subscirber
    ros::Subscriber mission_end_sub = nh.subscribe("mission_end", 1, missionEndCallback);


    //Publishers
    ros::Publisher location_updated_pub = nh.advertise<std_msgs::Int8>("location_updated", 1);

    // publisher and subscriber in same node (here)
    ros::Publisher update_location_pub = nh.advertise<std_msgs::Int8>("update_location", 1);

    // rate of 1 Hz
    //frequency that you would like to loop at. It will keep track of how long it has been since the last call to Rate::sleep(), and sleep for the correct amount of time.
    ros::Rate rate(2.0);

    

    // local global variable to get and set vote counter variable
    int vote_counter;

    int Needed;
    nh.getParam("/Needed", Needed);

    int updating_mission;

    //define standard sybc msg
    std_msgs::Int8 sync;
    sync.data = 1;

    // define TA states as enum
    enum STATES 
    {
        Idle, InSwarm, Leader, UpdatingLocation, WaitingSwarm    //check if this is correct or need strings

    } STATE;

    STATE = Idle;

    while(ros::ok()){

        ros::spinOnce();

        ROS_INFO("In while ros ok loop");

        switch(STATE){

            case Idle:
                // idle to in swarm
                if(update_status_var == 1 && ThisDrone->in_swarm(id)){
                    takeoff(3);
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
                    STATE = Idle;
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
                    update_location_pub.publish(sync);
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
                    location_updated_pub.publish(sync);
                    STATE = InSwarm;
                }
                break;

        }

    }
    














}
