#include <ros/ros.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <turtlesim/Pose.h>

/*
this broadcaster takes one argument for the /<turtlename>/pose Topic
that it's supposed to listen to and then "publishes" that coordinate
frame to the /tf Topic with the sendTransform function
*/

std::string turtle_name;

void poseCallback(const turtlesim::PoseConstPtr& msg){
  static tf2_ros::TransformBroadcaster br;  //create the broadcast object
  geometry_msgs::TransformStamped transformStamped; //create the message object
  
  //build the message object
  transformStamped.header.stamp = ros::Time::now();
  transformStamped.header.frame_id = "world";
  transformStamped.child_frame_id = turtle_name;
  transformStamped.transform.translation.x = msg->x;
  transformStamped.transform.translation.y = msg->y;
  transformStamped.transform.translation.z = 0.0;
  tf2::Quaternion q; //create the quaternion object
  q.setRPY(0, 0, msg->theta);
  transformStamped.transform.rotation.x = q.x();
  transformStamped.transform.rotation.y = q.y();
  transformStamped.transform.rotation.z = q.z();
  transformStamped.transform.rotation.w = q.w();

  br.sendTransform(transformStamped);
}

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf2_broadcaster"); //initialize node

  ros::NodeHandle private_node("~");
  if (! private_node.hasParam("turtle"))  // if the node has the parameter "turtle" then...
  {
    if (argc != 2){ROS_ERROR("which turtle pose do you want to subscribe to?"); return -1;};//make sure they included a turtle name
    turtle_name = argv[1];//assign the 2nd thing in argv as the turtlename
  }
  else//otherwise
  {
    private_node.getParam("turtle", turtle_name);
  }
    
  ros::NodeHandle node;
  //subscribe to the correct pose Topic and call the Callback function
  //this automatically passes the retrieved message to poseCallback
  ros::Subscriber sub = node.subscribe(turtle_name+"/pose", 10, &poseCallback);

  ros::spin();
  return 0;
};