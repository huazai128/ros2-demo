#include <stdio.h>
#include <stdlib.h>
#include <ros/ros.h>
#include "codbot_base/codbot_base.h"

int main(int argc,char **argv)
{
    ros::init(argc, argv, "codbot_base");
    ros::NodeHandle nh("~");

    CCodbotBase* codbotBase = new CCodbotBase(nh);

	
    static tf::TransformBroadcaster odom_broadcaster;
    geometry_msgs::TransformStamped odom_trans;
    nav_msgs::Odometry odom;
    std_msgs::Float32 wheel_left_speed_msg;
    std_msgs::Float32 wheel_right_speed_msg;
    geometry_msgs::Quaternion odom_quat;
	codbotBase->odom_pub = nh.advertise<nav_msgs::Odometry>(codbotBase->odom_topic, 5);
    //covariance matrix
    float covariance[36] = {0.01, 0, 0, 0, 0, 0,  
                            0, 0.01, 0, 0, 0, 0,  
                            0, 0, 1, 0, 0, 0,  
                            0, 0, 0, 1, 0, 0,  
                            0, 0, 0, 0, 1, 0,  
                            0, 0, 0, 0, 0, 0.01};  
    //load covariance matrix
    for (int i = 0; i < 36; i++) {
        odom.pose.covariance[i] = covariance[i];;
    }

    ros::Rate loop_rate(20); //10.0HZ
    while (nh.ok()) {
        //   if(myComDev.recv_update_flag==1)
        if (codbotBase->myOdomData.update_flag) {
            //odom_oriention trans to odom_quat
            odom_quat = tf::createQuaternionMsgFromYaw(codbotBase->myOdomData.oriention);//yaw trans quat
		
		//pub tf(odom->base_footprint)
		
            odom_trans.header.stamp = ros::Time::now();
            odom_trans.header.frame_id = codbotBase->odom_frame;
            odom_trans.child_frame_id = codbotBase->odom_child_frame;
            odom_trans.transform.translation.x = codbotBase->myOdomData.position_x;
            odom_trans.transform.translation.y = codbotBase->myOdomData.position_y;

            odom_trans.transform.translation.z = 0.0;
            odom_trans.transform.rotation = odom_quat;
			odom_broadcaster.sendTransform(odom_trans);
			
			//pub odom
            odom.header.stamp = ros::Time::now();
            odom.header.frame_id = codbotBase->odom_frame;
            odom.child_frame_id = codbotBase->odom_child_frame;
            odom.pose.pose.position.x = codbotBase->myOdomData.position_x;
            odom.pose.pose.position.y = codbotBase->myOdomData.position_y;

            odom.pose.pose.position.z = 0.0;
            odom.pose.pose.orientation = odom_quat;
            odom.twist.twist.linear.x = codbotBase->myOdomData.twist_x;
            odom.twist.twist.linear.y = codbotBase->myOdomData.twist_y;
	    odom.twist.twist.angular.z = codbotBase->myOdomData.twist_th;
            
            
            codbotBase->odom_pub.publish(odom);

        }
		ros::spinOnce();
        	loop_rate.sleep();
		
    }	
	
	
    return 0;
}
