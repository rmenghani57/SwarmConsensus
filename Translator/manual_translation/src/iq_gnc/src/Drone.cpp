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
    location_updated_var = location_updated.data;
}


void statusCallback(std_msgs::Int8 update_status){
    update_status_var = update_status.data;
}

void memberElectionCallback(std_msgs::Int8 member_election){
    member_election_var = member_election.data;
}

void leaderElectionCallback(std_msgs::Int8 leader_election){
    leader_election_var = leader_election.data;
}

void updateLocationCallback(std_msgs::Int8 update_location){
    update_location_var = update_location.data;
}

void missionEndCallback(std_msgs::Int8 mission_end){
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
    

    
    // This function takes our ros node handle as an input and initializes subcribers 
    // that will collect the necessary information from our autopilot. 
    // @returns n/a
    init_publisher_subscriber(nh);

    // wait for FCU connection
    wait4connect();
    
    // changing mode to GUIDED 
    set_mode("GUIDED");

	//create local reference frame 
	initialize_local_frame();
    
    std::string ThisNamespace;
    nh.getParam("namespace", ThisNamespace);
    ROS_INFO("THIS NAMESPACE IS: %s", ThisNamespace.c_str());

    // define subscribers
    ros::Subscriber update_status_sub;
    ros::Subscriber member_election_sub;
    ros::Subscriber leader_election_sub;
    ros::Subscriber update_location_sub;
    ros::Subscriber location_updated_sub;
    ros::Subscriber mission_end_sub;

    //Publishers
    ros::Publisher location_updated_pub = nh.advertise<std_msgs::Int8>("/location_updated", 1, true);  // checking without namespace and adding latch
    ros::Publisher update_location_pub = nh.advertise<std_msgs::Int8>("/update_location", 1, true);
    
    // local global variable to get and set vote counter variable
    int vote_counter;

    int Needed;

    int updating_mission;

    //define standard sybc msg
    std_msgs::Int8 sync;
    sync.data = 1;

    // define TA states as enum
    enum STATES 
    {
        Idle, InSwarm, Leader, UpdatingLocation, WaitingSwarm  

    } STATE;

    STATE = Idle;

    // create a flag to know when the drones voted when InSwarm state
    int flag = 0;

    ROS_INFO("Drones going into while loop");

    ros::Rate rate(0.5);

    while(ros::ok()){

        ros::spinOnce();
        
        switch(STATE){

            case Idle:
            {
                
                // idle to in swarm
                if(ThisDrone->in_swarm(id)){
                    
                    update_status_sub = nh.subscribe("/update_status", 1, statusCallback);
                    while(update_status_var != 1){
                        ros::spinOnce();
                    }
                                        
                    if(update_status_var == 1){
                        ROS_INFO("Inside Idle state, Drones should takeoff");
                        takeoff(10);
                        ROS_INFO("Waiting for drones to reach waypoint");
                        STATE = InSwarm;
                        update_status_sub.shutdown();
                        update_status_var = 0;
                    }
                    
                }
                rate.sleep();
                break;
            }     

            case InSwarm:
            {
                // member election logic 
                ROS_INFO("Drones InSwarm case");
                if(flag == 0 && ThisDrone->in_swarm(id)){  
                    
                    ROS_INFO("Drones in election loops");
                    member_election_sub = nh.subscribe("/member_election", 1, memberElectionCallback);
                    while(member_election_var != 1){
                        break;
                    }
                    nh.getParam("/vote_counter", vote_counter);
                    if(member_election_var == 1 && vote_counter < 3){  // vote counter restriction added here   
                        ThisDrone->vote_member(id);
                        vote_counter += 1;
                        nh.setParam("/vote_counter", vote_counter); 
                        nh.getParam("/vote_counter", vote_counter);
                        ROS_INFO("Drones voted for members: %d", vote_counter);
                        STATE = InSwarm;
                        member_election_sub.shutdown();
                        member_election_var = 0;
                        nh.getParam("/vote_counter", vote_counter);

                    }

                    while(vote_counter != 0){
                    // wait till member election sets votes back to 0
                        nh.getParam("/vote_counter", vote_counter);
                    }

                    ROS_INFO("drones out of member election, moving to leader election");
                    nh.getParam("/vote_counter", vote_counter);
                    leader_election_sub = nh.subscribe("/leader_election", 1, leaderElectionCallback);
                    while(leader_election_var != 1){
                        break;
                    }
                    if(leader_election_var == 1 && vote_counter < 3){       
                        ThisDrone->vote(id);
                        ros::Duration(3).sleep();
                        nh.getParam("/vote_counter", vote_counter);
                        ROS_INFO("Drones voted for leader: %d", vote_counter);
                        STATE = InSwarm;
                        leader_election_sub.shutdown();
                        leader_election_var = 0;
                        flag = 1;
                        break;    // bc leader drone at this point needs to go to leader state

                    }

                }

                // transition to leader
                nh.getParam("/vote_counter", vote_counter);
                nh.getParam("/updating_mission", updating_mission);
                if(ThisDrone->is_leader(id) && vote_counter == Needed && updating_mission == 1){    //inSwarm to leader transition
                    ThisDrone->update_leader_position(id);
                    STATE = Leader;
                    ROS_INFO("leader transitions");
                    rate.sleep();
                    break;
                }

                //transition to updating location 
                if(flag == 1 && ThisDrone->in_swarm(id) == true && ThisDrone->reached_goal(id) == false){  
                
                    update_location_sub = nh.subscribe("/update_location", 1, updateLocationCallback);
                    while(update_location_var != 1){
                        ros::spinOnce();
                    }
                    if(update_location_var == 1){    
                        ROS_INFO("Drone moves 1 unit");
                        ThisDrone->move(id);
                        vector<int> drone_location_x;
                        vector<int> drone_location_y;
                        nh.getParam("/drone_location_x", drone_location_x);
                        nh.getParam("/drone_location_y", drone_location_y);
                        set_destination(drone_location_x[id], drone_location_y[id], 10, 10);
                        ROS_INFO("Waiting for drones to reach waypoint");
                        STATE = UpdatingLocation;
                        update_location_sub.shutdown();
                        update_location_var = 0;
                        rate.sleep();
                        flag = 0;
                        break;
                    }   
                }

                // InSwarm to Idle transition
                if(ThisDrone->in_swarm(id) == false){    
                    update_status_sub = nh.subscribe("/update_status", 1, statusCallback);
                    if(update_status_var == 1){
                        land();
                        STATE = Idle;
                        update_status_sub.shutdown();
                        update_status_var = 0;
                        rate.sleep();
                        break;
                    }
                    
                }

                // Inswarm to idle transition
                mission_end_sub = nh.subscribe("/mission_end", 1, missionEndCallback);
                if(mission_end_var == 1){
                    land();
                    STATE = Idle;
                    mission_end_sub.shutdown();
                    mission_end_var = 0;
                    rate.sleep();
                    break;
                }
                
                ROS_INFO("breaking from inswarm case now");
                rate.sleep();
                break;
            }

            case Leader:
            {    
                // leader to in swarm state transition
                if(ThisDrone->is_leader(id) == false){
                    STATE = InSwarm;
                    rate.sleep();
                    break;
                }

                // leader to idle transition
                mission_end_sub = nh.subscribe("/mission_end", 1, missionEndCallback);
                if(mission_end_var == 1){
                    land();
                    STATE = Idle;
                    mission_end_sub.shutdown();
                    rate.sleep();
                    break;
                }
                
                // leader to waitingswarm transition
                nh.getParam("/updating_mission", updating_mission);
                if(ThisDrone->swarm_reached_goal() == false && ThisDrone->is_leader(id) && updating_mission == true){
                    while(update_location_pub.getNumSubscribers() < 2){
                        ROS_INFO("waiting for mission control update location sub");
                    }
                    update_location_pub.publish(sync); 
                    ROS_INFO("Leader Drone Moves 1 unit");
                    ThisDrone->move(id);
                    vector<int> drone_location_x;
                    vector<int> drone_location_y;
                    nh.getParam("/drone_location_x", drone_location_x);
                    nh.getParam("/drone_location_y", drone_location_y);
                    set_destination(drone_location_x[id], drone_location_y[id], 10, 10);
                    ROS_INFO("Waiting for drones to reach waypoint");
                    STATE = WaitingSwarm;
                    rate.sleep();
                    break;  
                }
                rate.sleep();
                break;
            }

            case UpdatingLocation:
            {
                //updating location to in swarm transition
                ROS_INFO("Drone updating location");
                location_updated_sub = nh.subscribe("/location_updated", 1, locationUpdatedCallback);
                while(location_updated_var != 1){
                    ros::spinOnce();
                }
                if(location_updated_var == 1){
                    STATE = InSwarm;
                    location_updated_sub.shutdown();
                    rate.sleep();
                    break;
                }
                rate.sleep();
                break;  
            }

            case WaitingSwarm:
            {
                
                while(location_updated_pub.getNumSubscribers() < 2){
                    ROS_INFO("waiting for mission control location updated sub");
                }
                    
                location_updated_pub.publish(sync);
                vote_counter = 0;
                nh.setParam("/vote_counter", vote_counter);
                STATE = InSwarm;
                // return flag to 0 for leader so it goes into election loop as well. 
                flag  = 0;
                rate.sleep();
                break;
            }
        }
       
        rate.sleep();
    }
    ROS_INFO("ROS IS NOT OKAY");
}
