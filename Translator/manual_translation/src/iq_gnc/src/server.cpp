#include "classes/server_base_class.cpp"
using namespace std;

int main(int argc, char** argv)
{
	ros::init(argc, argv, "server_node");
    ros::NodeHandle nh("~");
    Server* server_instance = new Server();
    int request_from_building, drone_number_counter, N, x_server, y_server;

    ros::Rate rate(1.0);
    while(ros::ok()) {
        ros::spinOnce();
        nh.getParam("/request_from_building", request_from_building);
        if(request_from_building) 
        {   
            ROS_INFO("Request from building detected!\n");
            
            nh.getParam("/drone_number_counter", drone_number_counter);
            nh.getParam("/N", N);
            if(drone_number_counter == N)
            {
                ROS_INFO("Guard Condition satisfied!!!\n");
                request_from_building = 0;
                nh.setParam("/request_from_building", request_from_building);
                nh.getParam("/x_server", x_server);
                nh.getParam("/y_server", y_server);

                // All the things on the transition
                server_instance->sendcoordinates(x_server, y_server);
                server_instance->encrypted_key(12345);
                nh.setParam("/request", 1);
            }
        }
        rate.sleep();
    }
    return 0;
}