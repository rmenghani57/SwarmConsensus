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

    //ROS_INFO("location_updated = [%d]", location_updated.data);
    location_updated_var = location_updated.data;

}


void statusCallback(std_msgs::Int8 update_status){

    //ROS_INFO("update_stautus = [%d]", update_status.data);
    update_status_var = update_status.data;

}

void memberElectionCallback(std_msgs::Int8 member_election){
    //ROS_INFO("member_election = [%d]", member_election.data);
    member_election_var = member_election.data;
}

void leaderElectionCallback(std_msgs::Int8 leader_election){
    //ROS_INFO("leader_election = [%d]", leader_election.data);
    leader_election_var = leader_election.data;
}

void updateLocationCallback(std_msgs::Int8 update_location){
    //ROS_INFO("update_location = [%d]", update_location.data);
    update_location_var = update_location.data;
}

void missionEndCallback(std_msgs::Int8 mission_end){
    //ROS_INFO("update_location = [%d]", mission_end.data);
    mission_end_var = mission_end.data;
}



int main(int argc, char** argv)
{

    
    // parse the argument passed in launch file to represent current drone id
    int id = atoi(argv[1]);

    // private node handle
	// initialize ROS
	ros::init(argc, argv, "drone_node");
	ros::NodeHandle nh("~");
	
    // instantiate Drone class
    Drone* ThisDrone = new Drone(id);
    

    // This function is called at the beginning of a program and will start of the communication links to the FCU. The function requires the program's ros nodehandle as an input 
    // This function takes our ros node handle as an input and initializes subcribers that will collect the necessary information from our autopilot. 
    // @returns n/a
    init_publisher_subscriber(nh);

    // wait for FCU connection
    wait4connect();
    
    // changing mode to GUIDED 
    set_mode("GUIDED");

    //if(current_state_g.mode == "GUIDED")
	//{
	//	ROS_INFO("Mode set to GUIDED. Mission starting");
    //}

	//create local reference frame 
	initialize_local_frame();
    
    std::string ThisNamespace;
    nh.getParam("namespace", ThisNamespace);
    ROS_INFO("THIS NAMESPACE IS: %s", ThisNamespace.c_str());

    //whenever new message in topic update_status, statusCallback func is called
    ros::Subscriber update_status_sub = nh.subscribe("/update_status", 1, statusCallback);
    //whenever new message in topic member_election, statusCallback func is called
    ros::Subscriber member_election_sub = nh.subscribe("/member_election", 1, memberElectionCallback);

    ros::Subscriber leader_election_sub = nh.subscribe("/leader_election", 1, leaderElectionCallback);

    // both pub and sub for this topic in same node, could cause problems
    ros::Subscriber update_location_sub = nh.subscribe("/update_location", 1, updateLocationCallback);


    // same as mission control template, might cause problems
    ros::Subscriber location_updated_sub = nh.subscribe("/location_updated", 1, locationUpdatedCallback);

    //mission end subscirber
    ros::Subscriber mission_end_sub = nh.subscribe("/mission_end", 1, missionEndCallback);


    //Publishers
    //ros::Publisher location_updated_pub = nh.advertise<std_msgs::Int8>((ThisNamespace+"/location_updated").c_str(), 1);  
    ros::Publisher location_updated_pub = nh.advertise<std_msgs::Int8>("/location_updated", 1, true);  // checking without namespace and adding latch
    // publisher and subscriber in same node (here)
    //ros::Publisher update_location_pub = nh.advertise<std_msgs::Int8>((ThisNamespace+"/update_location").c_str(), 1);
    ros::Publisher update_location_pub = nh.advertise<std_msgs::Int8>("/update_location", 1, true);

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

    ROS_INFO("Drones going into while loop");

    while(ros::ok()){


        ROS_INFO("ROS IS OK");

        ros::spinOnce();

        switch(STATE){

            case Idle:
                // idle to in swarm
                //ROS_INFO("Inside Idle 1, update_status_var: %d", update_status_var);
                //ROS_INFO("Drone in swarm? %d", ThisDrone->in_swarm(id));
                if(update_status_var == 1 && ThisDrone->in_swarm(id)){
                    ROS_INFO("Inside Idle state, Drones should takeoff");
                    takeoff(10);
                    STATE = InSwarm;
                }
                break;     

            case InSwarm:
                // member election logic
                ROS_INFO("Drones Inside InSwarm case");
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
                if(member_election_var == 1 && ThisDrone->in_swarm(id)){
                    ThisDrone->vote_member(id);
                    nh.getParam("/vote_counter", vote_counter);
                    vote_counter += 1;
                    nh.setParam("/vote_counter", vote_counter); 
                    ROS_INFO("Drones voted for members");
                    STATE = InSwarm;
                }
                else if(leader_election_var == 1 && ThisDrone->in_swarm(id)){    //leader election logic
                    ROS_INFO("Right before leader election");
                    ThisDrone->vote(id);
                    ROS_INFO("Drones voted for leader");
                    STATE = InSwarm;
                }
                else if(ThisDrone->is_leader(id) && vote_counter == Needed && updating_mission == 1){    //inSwarm to leader transition
                    ROS_INFO("before leader assigned");
                    ThisDrone->update_leader_position(id);
                    STATE = Leader;
                    ROS_INFO("leader assigned");
                }
                else if(ThisDrone->in_swarm(id) && ThisDrone->reached_goal(id) == false && update_location_var == 1){   // InSwarm to UpdatingLocation state
                    ROS_INFO("Drone moves 1 unit");
                    ThisDrone->move(id);
                    vector<int> drone_location_x;
                    vector<int> drone_location_y;
                    nh.getParam("/drone_location_x", drone_location_x);
                    nh.getParam("/drone_location_y", drone_location_y);
                    set_destination(drone_location_x[id], drone_location_y[id], 10, 10);
                    STATE = UpdatingLocation;
                }
                else if(ThisDrone->in_swarm(id) == false && update_status_var == 1){    // InSwarm to Idle transition
                    land();
                    STATE = Idle;
                }
                else if(mission_end_var == 1){
                    land();
                    STATE = Idle;
                }else{
                    STATE = InSwarm;
                }
        
                ROS_INFO("breaking from inswarm case now");
                break;

            case Leader:
                nh.getParam("/updating_mission", updating_mission);
                if(ThisDrone->is_leader(id) == false){
                    STATE = InSwarm;
                }
                else if(mission_end_var == 1){
                    land();
                    STATE = Idle;
                }
                else if(ThisDrone->swarm_reached_goal() == false && ThisDrone->is_leader(id) && updating_mission == true){
                    update_location_pub.publish(sync);
                    ROS_INFO("Leader Drone Moves 1 unit");
                    ThisDrone->move(id);
                    vector<int> drone_location_x;
                    vector<int> drone_location_y;
                    nh.getParam("/drone_location_x", drone_location_x);
                    nh.getParam("/drone_location_y", drone_location_y);
                    set_destination(drone_location_x[id], drone_location_y[id], 10, 10);
                    STATE = WaitingSwarm;
                }else{
                    STATE = Leader;
                }

                break;

            case UpdatingLocation:
                ROS_INFO("Drone updating location");
                if(location_updated_var == 1){
                    STATE = InSwarm;
                }
                break;  

            case WaitingSwarm:
                vote_counter = 0;
                nh.setParam("/vote_counter", vote_counter);
                location_updated_pub.publish(sync);
                STATE = InSwarm;    
                break;

        }

    }

    ROS_INFO("ROS IS NOT OKAY");


}
