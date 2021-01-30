#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>

std::string listen_turtle_name;
std::string create_turtle_name;

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf2_listener");

  ros::NodeHandle node;
  if (!node.hasParam("lturtle")||!node.hasParam("cturtle")){
      if (argc != 3){ROS_ERROR("need two turtle names in args: listen, create"); return -1;};
      listen_turtle_name = argv[1]; //name of the turtle to listen to
      create_turtle_name = argv[2]; //name of turtle to create
  }
  else {
      node.getParam("lturtle",listen_turtle_name);
      node.getParam("cturtle",create_turtle_name);
  }

  ros::service::waitForService("spawn");   //wait for spawn service
  ros::ServiceClient spawner =
    node.serviceClient<turtlesim::Spawn>("spawn"); //create a client to call the service
  turtlesim::Spawn turtle;  //object to spawn
  turtle.request.x = 4;
  turtle.request.y = 2;
  turtle.request.theta = 0;
  turtle.request.name = create_turtle_name;
  spawner.call(turtle); // spawn the turtle

  //advertise that this node publishes a Twist message to turtle2/cmd_vel at 10hz
  ros::Publisher turtle_vel =
    node.advertise<geometry_msgs::Twist>(create_turtle_name+"/cmd_vel", 10);

  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);  //create a listener

  ros::Rate rate(10.0);
  while (node.ok()){
    geometry_msgs::TransformStamped transformStamped;
    try{
      transformStamped = tfBuffer.lookupTransform(create_turtle_name, listen_turtle_name,
                               ros::Time(0));//try to get the latest message
    }
    catch (tf2::TransformException &ex) {
      ROS_WARN("%s",ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }

    geometry_msgs::Twist vel_msg;

    vel_msg.angular.z = 4.0 * atan2(transformStamped.transform.translation.y,
                                    transformStamped.transform.translation.x);
    vel_msg.linear.x = 0.5 * sqrt(pow(transformStamped.transform.translation.x, 2) +
                                  pow(transformStamped.transform.translation.y, 2));
    turtle_vel.publish(vel_msg);

    rate.sleep();
  }
  return 0;
};