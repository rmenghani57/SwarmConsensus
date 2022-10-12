#include "classes/input_base_class.cpp"
using namespace std;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "input_node");
    ros::NodeHandle nh("~");
    int counter = 0;
    ros::Rate rate(1.0);
    Input* input_instance = new Input();

    // Debug variables to check if service is being generated.
    int debug_x, debug_y, service_interval = 5;

    while(ros::ok())
    {
        ros::spinOnce();
        
        if(counter == service_interval)
        {
            ROS_INFO("GENERATING REQUEST!\n");
            input_instance->generate_request();
        }
        else if(counter > service_interval)
        {
            counter = 0;
            service_interval = 100;
            nh.getParam("/x_server", debug_x);
            nh.getParam("/y_server", debug_y);
            ROS_INFO("I wish to be serviced at %d %d\n", debug_x, debug_y);
        }
        else
        {
            ROS_INFO("T minus %d to service request\n", service_interval - counter);
        }
        counter++;
        rate.sleep();
    }
}