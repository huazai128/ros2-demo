#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <sensor_msgs/Imu.h>
#include "tf/transform_broadcaster.h"
#include <tf/tf.h>
#include "nav_msgs/Odometry.h"
#include "std_msgs/Float32.h"
#include <vector>

//serial com
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
//multi thread
#include <pthread.h>
//string
#include <string>
#include <cstring>
//cos,sin
#include <math.h>
//eigen
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

using namespace std;

class CCodbotBase{
public:
    CCodbotBase(ros::NodeHandle node);
    bool init();
    void GetParams();
    void SetGorgePro(const char *serial_num);
    int set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop);
    void DiffCallBack(const geometry_msgs::Twist & cmd_input);
    void AckerCallBack(const geometry_msgs::Twist & cmd_input);
	void MecanumCallBack(const geometry_msgs::Twist & cmd_input);
    static void *MyReadThread(void *pt);
    static void *MyWriteThread(void *th);
    void ReceiveUpdate(int data_len, unsigned char *data_type, unsigned char new_buff);
    bool CheckRawData(unsigned char *raw_data, int data_num, int current_code);
	
public:
    ros::Publisher odom_pub;
private:
    ros::Publisher imu_pub;
    ros::Subscriber cmd_sub;
    ros::NodeHandle _nh;

    //params:cmd_port cmd_topic odom_topic imu_topic imu_frame odom_frame child_frame baud_rate
    string cmd_port;
    string cmd_topic;
    string imu_topic;
    string imu_frame;
    string baud_rate;
    int debug_num;
	double coeff_odom_x;
	double coeff_odom_y;	
	
    const string alltype[4] = {"A100","D100","T100","M100"};
    string current_type;
    string feedback_type;
private:

    pthread_t th_base_write;


    struct BaseReader
    {
        int serialNum;
        unsigned char carVelRaw[13]  = {0xfd, 0x09, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char motorVelRaw[12] = {0x00};
        unsigned char batteryRaw[5]  = {0x00};
        unsigned char imuRpyRaw[13]     = {0xfd, 0x09, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char imuRaw[34]  = {0x00};
        unsigned char carTypeRaw[5]  = {0x00};

        unsigned char newMsgRaw[250];
        unsigned char writebuff[13]={0xcd, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char stopbuff[13]={0xcd, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        int nread = 0;
        int nwrite = 0;
        bool recv_update_flag=0;
        bool send_update_flag=0;
    }baseCommunication;

    struct BaseData
    {
        float vx;
        float vy;
        float yaw;
        float v1;
        float v2;
        float v3;
        float v4;
        float init_yaw;
        float curr_orien;
	float angler_speed;
    }currentBaseData;
public:
	string odom_frame;
	string odom_topic;
    string odom_child_frame;
    struct OdomData
    {
        //motor params
        float speed_ratio = 1.2941; //unit:
        float wheel_distance=0.19; //unit: m
        float encode_sampling_time=0.04; //unit: s
        float cmd_vel_linear_max; //unit: m/s
		float cmd_vel_linear_min; //unit: m/s
        float cmd_vel_anglar_max; //unit: rad/s
		float cmd_vel_anglar_min; //unit: rad/s
        float cmd_vel_angle_max; //unit: rad
		float cmd_vel_angle_min; //unit: rad
        //odom result
        float position_x=0.0; //unit: m
        float position_y=0.0; //unit: m
        float twist_x = 0.0;
        float twist_y = 0.0;
		float twist_th = 0.0;
        float oriention=0.0; //unit: rad
        float velocity_linear=0.0; //unit: m/s
        float velocity_angular=0.0; //unit: rad/s
        bool update_flag = false;
    }myOdomData;
};
