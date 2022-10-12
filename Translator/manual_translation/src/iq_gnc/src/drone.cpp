#include "classes/drone_base_class.cpp"
#include <thread>
#include <chrono>
using namespace std;

void service(bool ret, int my_number, ros::NodeHandle nh) {
    std::vector<gnc_api_waypoint> waypointList;
    gnc_api_waypoint nextWayPoint;
    vector<int> selected_drone;
    nh.getParam("/selected_drone", selected_drone);
    if(selected_drone[0] == my_number || ret)
    {
        //initialize control publisher/subscribers
        init_publisher_subscriber(nh);

        // wait for FCU connection
        wait4connect();

        //wait for used to switch to mode GUIDED
        // wait4start();

        // changing mode to GUIDED programmatically assuming drone is ready for flight
        set_mode("GUIDED");
        if(!ret) {
            //create local reference frame 
            initialize_local_frame();
        }

        //request takeoff
        takeoff(3);

        // First way point is the point in space after takeoff
        nextWayPoint.x = 0;
        nextWayPoint.y = 0;
        nextWayPoint.z = 3;
        nextWayPoint.psi = 0;
        waypointList.push_back(nextWayPoint);

        int x_server, y_server;
        if(!ret) {
            nh.getParam("/x_server", x_server);
            nh.getParam("/y_server", y_server);
        } else {
            nh.getParam("/x", x_server);
            nh.getParam("/y", y_server);
        }
        vector<double> x;
        vector<double> y;
        nh.getParam("/x", x);
        nh.getParam("/y", y);
        x_server = x[my_number] - x_server;
        y_server = y[my_number] - y_server;
        nextWayPoint.x = x_server;
        nextWayPoint.y = y_server;
        nextWayPoint.z = 3;
        nextWayPoint.psi = 0;
        waypointList.push_back(nextWayPoint);
    }

    ros::Rate rate(1.0);
    int counter = 0;
    while(ros::ok())
    {  
        ros::spinOnce();
        if(selected_drone[0] == my_number)
        {
            if(check_waypoint_reached(.3) == 1)
            {
                if (counter < waypointList.size())
                {
                    set_destination(waypointList[counter].x,waypointList[counter].y,waypointList[counter].z, waypointList[counter].psi);
                    counter++;	
                }else{
                    //land after all waypoints are reached
                    land();
                    this_thread::sleep_for(chrono::seconds(10));
                    break;
                }	
            }	
        }
        else
        {
            ROS_INFO("I, Drone %d, am waiting for another service call", my_number);
        }
        rate.sleep();
    }
    return;
}

int main(int argc, char** argv)
{
    //initialize ros 
	ros::init(argc, argv, "drone_node");
    ros::NodeHandle nh("~");
    int my_number;
    // the following two lines are for debug, can be removed when done.
    bool got_my_number = false;
    int random_drone_number = rand() % 100 + 1;

    /*  
        Assigning a number for each spawned node so as to be able to 
        instantiate their own drone class.
    */
    nh.getParam("/drone_number", my_number);
    nh.setParam("/drone_number", my_number + 1);
    got_my_number = true;
    Drone* drone_instance = new Drone(my_number);
    
    // Has the same effect as incrementing drone counter as defined in the drone template.
    nh.setParam("/drone_number_counter", my_number + 1);

    int request;

    ros::Rate rate(1.0);
    while(ros::ok()) {
        ros::spinOnce();
        nh.getParam("/request", request);
        if(request)
        {
            int drone_number_counter;
            int N;
            nh.getParam("/drone_number_counter", drone_number_counter);
            nh.getParam("/N", N);
            if(drone_instance->check_availability() && drone_number_counter == N)
            {
                drone_instance->decrypt();
                nh.setParam("/drone_synchronisation_counter", 0);
                if(drone_instance->checksensor() && drone_instance->battery() >= 50)
                {
                    if(drone_instance->check_authenticity())
                    {
                        vector<double> dist;
                        vector<double> x;
                        vector<double> y;
                        for(int i = 0; i < 100; i++)
                        {
                            nh.getParam("/dist", dist);
                            nh.getParam("/x", x);
                            nh.getParam("/y", y);
                            dist[my_number] = drone_instance->linear_distance(x[my_number], y[my_number]);

                            nh.setParam("/dist", dist);
                            drone_instance->update_distance(dist[my_number]);
                        }
                        
                        nh.getParam("/drone_number_counter", drone_number_counter);
                        nh.setParam("/drone_number_counter", drone_number_counter - 1);
                        // ROS_INFO("Updated Drone %d's distance to service point!", my_number);
                    }
                }
            }
            else 
            {
                nh.getParam("/drone_number_counter", drone_number_counter);
                if(drone_number_counter == 0)
                {   
                    for(int i = 0; i < 100; i++)
                    {
                        drone_instance->update_shortest_distance();
                    }
                    // ROS_INFO("UPDATED WHO Drone %d thinks is the closest!", my_number);
                    
                    int drone_synchronisation_counter;
                    nh.getParam("/drone_synchronisation_counter", drone_synchronisation_counter);
                    nh.setParam("/drone_synchronisation_counter", drone_synchronisation_counter + 1);

                    request = 0;
                    nh.setParam("/request", 0);
                }
            }
        }
        else
        {
            int N;
            int drone_synchronisation_counter;
            nh.getParam("/N", N);
            nh.getParam("/drone_synchronisation_counter", drone_synchronisation_counter);
            if(drone_synchronisation_counter == N)
            {
                vector<int> selected_drone;
                nh.getParam("/selected_drone", selected_drone);
                if(selected_drone[0] != my_number)// probably replace with drone_instance->i)
                {
                    int drone_synchronisation_counter_2;
                    nh.getParam("/drone_synchronisation_counter_2", drone_synchronisation_counter_2);
                    nh.setParam("/drone_synchronisation_counter_2", drone_synchronisation_counter_2 + 1);
                    ROS_INFO("I, Drone %d am not the closest!", my_number);
                    break;
                }
                else
                {
                    vector<bool> availability;
                    nh.getParam("/availability", availability);
                    availability[my_number] = false;
                    nh.setParam("/availability", availability);
                    break;
                }
            }
        }
        rate.sleep();
    }

    service(false, my_number, nh);
    service(true, my_number, nh);
    return 0;
}
