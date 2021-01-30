#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf2_listener");

  ros::NodeHandle node;


  ros::service::waitForService("spawn");   //wait for spawn service
  ros::ServiceClient spawner =
    node.serviceClient<turtlesim::Spawn>("spawn"); //create a client to call the service
  turtlesim::Spawn turtle;  //object to spawn
  turtle.request.x = 8;
  turtle.request.y = 4;
  turtle.request.theta = 0;
  turtle.request.name = "turtle3";
  spawner.call(turtle); // spawn the turtle

  //advertise that this node publishes a Twist message to turtle2/cmd_vel at 10hz
  ros::Publisher turtle_vel =
    node.advertise<geometry_msgs::Twist>("turtle3/cmd_vel", 10);

  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);  //create a listener

  ros::Rate rate(10.0);
  while (node.ok()){
    geometry_msgs::TransformStamped transformStamped;
    try{
      transformStamped = tfBuffer.lookupTransform("turtle3", "turtle2",
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