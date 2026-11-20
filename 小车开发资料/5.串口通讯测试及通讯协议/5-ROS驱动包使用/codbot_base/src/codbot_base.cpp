#include "codbot_base/codbot_base.h"

CCodbotBase::CCodbotBase(ros::NodeHandle node)
:_nh(node)
{
    init();
}
/*initialize codbot_base node */
bool CCodbotBase::init() {

    GetParams();


    if (current_type == alltype[0]) {
        std::cout << "Current chassis type is ackerman" << std::endl;
	cmd_sub = _nh.subscribe(cmd_topic, 10, &CCodbotBase::AckerCallBack, this);
    } else if (current_type == alltype[1] || current_type == alltype[2]) {
        std::cout << "Current chassis type is differential/tan" << std::endl;
        cmd_sub = _nh.subscribe(cmd_topic, 10, &CCodbotBase::DiffCallBack,this);
    }else if (current_type == alltype[3]){
		cmd_sub = _nh.subscribe(cmd_topic, 10, &CCodbotBase::MecanumCallBack,this);
	}else{
		cout << "\033[31m can not find  " << current_type<< " chassis" << endl;
		return false;
	}
    imu_pub = _nh.advertise<sensor_msgs::Imu>(imu_topic, 20);

    SetGorgePro(cmd_port.c_str());
    //baseCommunication.serialNum = open(cmd_port.c_str(), O_RDWR);
    //set_serial(baseCommunication.serialNum,115200,8,'N',1);

    pthread_t th_base_read;
    int th_read_status = pthread_create(&th_base_read, NULL, MyReadThread, this);
    if (0 != th_read_status) {
        cout << "\033[31mcan't create base read thread: " << th_read_status << endl;
        return false;
    }

    pthread_t th_base_wirte;
    int th_write_status = pthread_create(&th_base_write, NULL, MyWriteThread, this);
    if (0 != th_write_status) {
        cout << "\033[31mcan't create base write thread: " << th_base_wirte << endl;
        return false;
    }

}
/*get launch params*/

void CCodbotBase::GetParams() {
    _nh.param<std::string>("cmd_port",cmd_port,"/dev/base");
    _nh.param<std::string>("baud_rate",baud_rate,"B115200");
    _nh.param<std::string>("cmd_vel_topic",cmd_topic,"/cmd_vel");
    _nh.param<std::string>("odom_topic",odom_topic,"/odom");
    _nh.param<std::string>("odom_frame",odom_frame,"/odom");
    _nh.param<std::string>("odom_child_frame",odom_child_frame,"/base_footprint");
    _nh.param<std::string>("imu_topic",imu_topic,"/imu");
    _nh.param<std::string>("imu_frame",imu_frame,"/imu_link");
    _nh.param<int>("debug_num",debug_num,0);
	_nh.param<double>("coeff_odom_x",coeff_odom_x,1.0);
	_nh.param<double>("coeff_odom_y",coeff_odom_y,1.0);
    _nh.param<std::string>("current_type",current_type,"D100");
	_nh.param<float>("cmd_vel_linear_max",myOdomData.cmd_vel_linear_max,1.0);
	_nh.param<float>("cmd_vel_linear_min",myOdomData.cmd_vel_linear_min,0.1);
	_nh.param<float>("cmd_vel_anglar_max",myOdomData.cmd_vel_anglar_max,2.0);
	_nh.param<float>("cmd_vel_anglar_min",myOdomData.cmd_vel_anglar_min,0.3);
	_nh.param<float>("cmd_vel_angle_max",myOdomData.cmd_vel_angle_max,25);
	_nh.param<float>("cmd_vel_angle_min",myOdomData.cmd_vel_angle_min,10);

}

/*set com device attribute*/

void CCodbotBase::SetGorgePro(const char *serial_num) {

    baseCommunication.serialNum = open(serial_num, O_RDWR);
    if(-1 == baseCommunication.serialNum ){
        cout << "\033[31mOpen "<<serial_num<<" \033[31mserial port failed!"<<endl;
        return ;
    }

    struct termios my_options;
    if( 0 != tcgetattr(baseCommunication.serialNum, &my_options )){
        cout << "\033[31mGet serial port attribute failed" << endl;
        return ;
    }
    int convert_baud;
    tcflush(baseCommunication.serialNum, TCIFLUSH);

    convert_baud = atoi(baud_rate.c_str());
    cfsetispeed(&my_options, B115200);
    cfsetospeed(&my_options, B115200);

    my_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//
    my_options.c_oflag &= ~OPOST;
    my_options.c_iflag &= ~(IXON | IXOFF | IXANY);
    my_options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    my_options.c_oflag &= ~(ONLCR | OCRNL);

    if(0 != tcsetattr(baseCommunication.serialNum, TCSANOW, &my_options)){
        cout << "\033[31mSet "<<serial_num<<" \033[31mserial port failed!"<<endl;
    }

}


int CCodbotBase::set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
    struct termios newttys1,oldttys1;

    /*保存原有串口配置*/
    if(tcgetattr(fd,&oldttys1)!=0)
    {
        perror("Setup IMU Serial ");
        return -1;
    }

//    memset(&newttys1,0,sizeof(newttys1));/* 先将新串口配置清0 */
    newttys1.c_cflag|=(CLOCAL|CREAD ); /* CREAD 开启串行数据接收，CLOCAL并打开本地连接模式 */

//    newttys1.c_cflag &=~CSIZE;/* 设置数据位 */
    /* 数据位选择 */
    switch(nBits)
    {
        case 7:
            newttys1.c_cflag |=CS7;
            break;
        case 8:
            newttys1.c_cflag |=CS8;
            break;
        default:
            newttys1.c_cflag |=CS8;
            break;
    }
    /* 设置奇偶校验位 */
    switch( nEvent )
    {
        case '0':  /* 奇校验 */
            newttys1.c_cflag |= PARENB;/* 开启奇偶校验 */
            newttys1.c_iflag |= (INPCK | ISTRIP);/*INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特  */
            newttys1.c_cflag |= PARODD;/*启用奇校验(默认为偶校验)*/
            break;
        case 'E':/*偶校验*/
            newttys1.c_cflag |= PARENB; /*开启奇偶校验  */
            newttys1.c_iflag |= ( INPCK | ISTRIP);/*打开输入奇偶校验并去除字符第八个比特*/
            newttys1.c_cflag &= ~PARODD;/*启用偶校验*/
            break;
        case 'N': /*无奇偶校验*/
            newttys1.c_cflag &= ~PARENB;
            break;
        default:
            newttys1.c_cflag &= ~PARENB;
            break;
    }
    /* 设置波特率 */
    switch( nSpeed )
    {
        case 2400:
            cfsetispeed(&newttys1, B2400);
            cfsetospeed(&newttys1, B2400);
            break;
        case 4800:
            cfsetispeed(&newttys1, B4800);
            cfsetospeed(&newttys1, B4800);
            break;
        case 9600:
            cfsetispeed(&newttys1, B9600);
            cfsetospeed(&newttys1, B9600);
            break;
        case 115200:
            cfsetispeed(&newttys1, B115200);
            cfsetospeed(&newttys1, B115200);
            break;
        case 230400:
            cfsetispeed(&newttys1, B230400);
            cfsetospeed(&newttys1, B230400);
            break;
        case 460800:
            cfsetispeed(&newttys1, B460800);
            cfsetospeed(&newttys1, B460800);
            break;
        default:
            cfsetispeed(&newttys1, B9600);
            cfsetospeed(&newttys1, B9600);
            break;
    }
    /*设置停止位*/
    if( nStop == 1)/* 设置停止位；若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB */
    {
        newttys1.c_cflag &= ~CSTOPB;/*默认为一位停止位； */
    }
    else if( nStop == 2)
    {
        newttys1.c_cflag |= CSTOPB;/* CSTOPB表示送两位停止位 */
    }

    /* 设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时*/
    newttys1.c_cc[VTIME] = 0;/* 非规范模式读取时的超时时间；*/
    newttys1.c_cc[VMIN]  = 0; /* 非规范模式读取时的最小字符数*/
    tcflush(fd ,TCIFLUSH);/* tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */

    newttys1.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//
    newttys1.c_oflag &= ~OPOST;
    newttys1.c_iflag &= (IXON | IXOFF );
    newttys1.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    newttys1.c_oflag &= ~(ONLCR | OCRNL);
    /*激活配置使其生效*/
    if((tcsetattr( fd, TCSANOW,&newttys1))!=0)
    {
        perror("IMU com set error");
        exit(1);
    }

    return 0;
}


void CCodbotBase::DiffCallBack(const geometry_msgs::Twist & cmd_input) {

    float linear_speed;
    float angular_speed;
    linear_speed = cmd_input.linear.x ;//m/s
    angular_speed = cmd_input.angular.z ;//rad/s

    //motor linear speed limit
	double linear_sign = 0.0;
	if(linear_speed>0){
		linear_sign = 1.0;
	}else if(linear_speed<0){
		linear_sign = -1.0;
	}else{
		linear_sign = 0;
	}	
	linear_speed = min(abs(linear_speed),myOdomData.cmd_vel_linear_max);
	linear_speed = max(abs(linear_speed),myOdomData.cmd_vel_linear_min);
	linear_speed = linear_sign*linear_speed;
	//motor angular speed limit
	double angular_sign = 0.0;
	if(angular_speed>0){
		angular_sign = 1.0;
	}else if(angular_speed<0){
		angular_sign = -1.0;
	}else{
		angular_sign = 0;
	}	
	angular_speed = min(abs(angular_speed),myOdomData.cmd_vel_anglar_max);
	angular_speed = max(abs(angular_speed),myOdomData.cmd_vel_anglar_min);
	angular_speed = angular_sign*angular_speed;

    while(baseCommunication.send_update_flag!=0);//wait for flag clear
    //left motor enc set
    baseCommunication.writebuff[0] = 0xCD;
    baseCommunication.writebuff[1] = 0x0A;
    baseCommunication.writebuff[2] = 0x01;
    if(linear_speed>=0)
        baseCommunication.writebuff[3] = 0x00;
    else
    {
        baseCommunication.writebuff[3] = 0x01;
        linear_speed = -linear_speed;
    }
    baseCommunication.writebuff[4] = (int(linear_speed*1000)>>8)&0xff;
    baseCommunication.writebuff[5] = int(linear_speed*1000)&0xff;

    baseCommunication.writebuff[6] = 0x00;
    baseCommunication.writebuff[7] = 0x00;
    baseCommunication.writebuff[8] = 0x00;

    //right motor enc set
    if(angular_speed > 0)
        baseCommunication.writebuff[9] = 0x00;
    else
    {
        baseCommunication.writebuff[9] = 0x01;
        angular_speed *= -1;
    }

    baseCommunication.writebuff[10] = (int(angular_speed*100)>>8)&0xff;
    baseCommunication.writebuff[11] = int(angular_speed*100)&0xff;
    //create checksum
    baseCommunication.writebuff[12] = 0x00;
    for (int i = 2; i < 12; i++)
    {
        baseCommunication.writebuff[12] ^= baseCommunication.writebuff[i];
    }

    baseCommunication.send_update_flag=1; //set flag
}

void CCodbotBase::AckerCallBack(const geometry_msgs::Twist & cmd_input) {

    float linear_x,angle;
	
    linear_x = cmd_input.linear.x;//m/s
    angle = cmd_input.angular.z;//rad
	angle = angle*180/M_PI;
    //motor linear speed limit
	double linear_sign = 1.0;
	if(linear_x>0){
		linear_sign = 1.0;
	}else if(linear_x<0){
		linear_sign = -1.0;
	}	
	linear_x = min(abs(linear_x),myOdomData.cmd_vel_linear_max);
	linear_x = max(abs(linear_x),myOdomData.cmd_vel_linear_min);
	linear_x = linear_sign*linear_x;
	//motor angular speed limit
    double angle_sign = 1.0;
	if(angle>0){
		angle_sign = 1.0;
	}else if(angle<0){
		angle_sign = -1.0;
	}	
	angle = min(abs(angle),myOdomData.cmd_vel_angle_max);
	angle = max(abs(angle),myOdomData.cmd_vel_angle_min);
	angle = (linear_sign !=0)?angle_sign*linear_sign*angle:angle_sign*angle;//reversing the reversing

    while(baseCommunication.send_update_flag!=0);//wait for flag clear

    baseCommunication.writebuff[0] = 0xCD;
    baseCommunication.writebuff[1] = 0x0A;
    baseCommunication.writebuff[2] = 0x01;
    if(linear_x >= 0)
        baseCommunication.writebuff[3] = 0x00;
    else
    {
        baseCommunication.writebuff[3] = 0x01;
        linear_x *= -1;
    }

    baseCommunication.writebuff[4] = (int(linear_x*1000)>>8)&0xff;
    baseCommunication.writebuff[5] = int(linear_x*1000)&0xff;


    baseCommunication.writebuff[6] = 0x00;
    baseCommunication.writebuff[7] = 0x00;
    baseCommunication.writebuff[8] = 0x00;

    //right motor enc set
    if(angle >= 0)
        baseCommunication.writebuff[9] = 0x00;
    else
    {
        baseCommunication.writebuff[9] = 0x01;
        angle *= -1;
    }

    baseCommunication.writebuff[10] = (int(angle)>>8)&0xff;
    baseCommunication.writebuff[11] = int(angle)&0xff;
    //create checksum
    baseCommunication.writebuff[12] = 0x00;
    for (int i = 2; i < 12; i++)
    {
        baseCommunication.writebuff[12] ^= baseCommunication.writebuff[i];
    }

    baseCommunication.send_update_flag=1; //set flag
}

void CCodbotBase::MecanumCallBack(const geometry_msgs::Twist & cmd_input) {

    float linear_x_speed;
	float linear_y_speed;
    float angular_speed;
    linear_x_speed = cmd_input.linear.x ;//m/s
	linear_y_speed = cmd_input.linear.y;
    angular_speed = cmd_input.angular.z ;//rad/s

    //motor max vel limit
    float linear_max_limit = myOdomData.cmd_vel_linear_max;
    float angular_max_limit = myOdomData.cmd_vel_anglar_max;
    if(linear_x_speed>linear_max_limit)
        linear_x_speed = linear_max_limit;
    if(linear_x_speed<(-1*linear_max_limit))
        linear_x_speed = -1*linear_max_limit;
    if(angular_speed>angular_max_limit)
        angular_speed = angular_max_limit;
    if(angular_speed<(-1*angular_max_limit))
        angular_speed = -1*angular_max_limit;

    int delta_encode_left_temp;
    int delta_encode_right_temp;
    delta_encode_left_temp = (linear_x_speed-0.5*(myOdomData.wheel_distance)*angular_speed)*(myOdomData.encode_sampling_time)/(myOdomData.speed_ratio);
    delta_encode_right_temp = (linear_x_speed+0.5*(myOdomData.wheel_distance)*angular_speed)*(myOdomData.encode_sampling_time)/(myOdomData.speed_ratio);
    while(baseCommunication.send_update_flag!=0);//wait for flag clear
    //left motor enc set
    baseCommunication.writebuff[0] = 0xCD;
    baseCommunication.writebuff[1] = 0x0A;
    baseCommunication.writebuff[2] = 0x01;
    if(linear_x_speed>=0)
        baseCommunication.writebuff[3] = 0x00;
    else
    {
        baseCommunication.writebuff[3] = 0x01;
        linear_x_speed = -linear_x_speed;
    }
    baseCommunication.writebuff[4] = (int(linear_x_speed*1000)>>8)&0xff;
    baseCommunication.writebuff[5] = int(linear_x_speed*1000)&0xff;
	
	if(linear_y_speed>=0)
        baseCommunication.writebuff[6] = 0x00;
    else
    {
        baseCommunication.writebuff[6] = 0x01;
        linear_y_speed = -linear_y_speed;
    }
    baseCommunication.writebuff[7] = (int(linear_y_speed*1000)>>8)&0xff;
    baseCommunication.writebuff[8] = int(linear_y_speed*1000)&0xff;

    //right motor enc set
    if(angular_speed > 0)
        baseCommunication.writebuff[9] = 0x00;
    else
    {
        baseCommunication.writebuff[9] = 0x01;
        angular_speed *= -1;
    }

    baseCommunication.writebuff[10] = (int(angular_speed*100)>>8)&0xff;
    baseCommunication.writebuff[11] = int(angular_speed*100)&0xff;
    //create checksum
    baseCommunication.writebuff[12] = 0x00;
    for (int i = 2; i < 12; i++)
    {
        baseCommunication.writebuff[12] ^= baseCommunication.writebuff[i];
    }

    baseCommunication.send_update_flag=1; //set flag
}

void *CCodbotBase::MyReadThread(void *pt) {
    CCodbotBase *pt_read = (CCodbotBase*)pt;//通过this指针，间接访问类的非静态成员
    unsigned char read_buff;
    while(true){
        int msg_len=0;
        double dt;
        int nread = read(pt_read->baseCommunication.serialNum,&read_buff ,1);
        if( nread > 0){
            pt_read->ReceiveUpdate(13,pt_read->baseCommunication.carVelRaw,read_buff);
            pt_read->ReceiveUpdate(12,pt_read->baseCommunication.motorVelRaw,read_buff);
            pt_read->ReceiveUpdate(5,pt_read->baseCommunication.batteryRaw,read_buff);
            pt_read->ReceiveUpdate(13,pt_read->baseCommunication.imuRpyRaw,read_buff);
            pt_read->ReceiveUpdate(34,pt_read->baseCommunication.imuRaw,read_buff);
            pt_read->ReceiveUpdate(5,pt_read->baseCommunication.carTypeRaw,read_buff);

            if( (0xfd == pt_read->baseCommunication.carVelRaw[0]) && (0x0a == pt_read->baseCommunication.carVelRaw[1]) &&
                (0x02 == pt_read->baseCommunication.carVelRaw[2])){
                if( pt_read->CheckRawData(pt_read->baseCommunication.carVelRaw, 13, 2) ){
                    pt_read->baseCommunication.recv_update_flag = false;
                    static double current_time = ros::Time::now().toSec();
                    static double previous_time = ros::Time::now().toSec();
                    current_time = ros::Time::now().toSec();
                    dt = current_time - previous_time;
                    previous_time = current_time;
					
                    pt_read->currentBaseData.vx = (pt_read->baseCommunication.carVelRaw[3]>0?-1:1)*(pt_read->baseCommunication.carVelRaw[4]*256 +pt_read->baseCommunication.carVelRaw[5]) / 1000.0;
                    pt_read->currentBaseData.vy = (pt_read->baseCommunication.carVelRaw[6]>0?-1:1)*(pt_read->baseCommunication.carVelRaw[7]*256+pt_read->baseCommunication.carVelRaw[8]) / 1000.0;
					pt_read->currentBaseData.angler_speed = (pt_read->baseCommunication.carVelRaw[9]>0?-1:1)*(pt_read->baseCommunication.carVelRaw[10]*256+pt_read->baseCommunication.carVelRaw[11]) / 100.0;
					pt_read->currentBaseData.vx = abs(pt_read->currentBaseData.vx) > 0.01 ? pt_read->currentBaseData.vx : 0;
					pt_read->currentBaseData.vy = abs(pt_read->currentBaseData.vy) > 0.01 ? pt_read->currentBaseData.vy : 0;
					pt_read->currentBaseData.angler_speed = abs(pt_read->currentBaseData.angler_speed) > 0.01 ? pt_read->currentBaseData.angler_speed : 0;
                    pt_read->baseCommunication.recv_update_flag=true;
                }
            }

            if( (0xfd == pt_read->baseCommunication.motorVelRaw[0]) && (0x09 == pt_read->baseCommunication.motorVelRaw[1]) &&(0x03 == pt_read->baseCommunication.motorVelRaw[2])){
                if( pt_read->CheckRawData(pt_read->baseCommunication.motorVelRaw, 12, 3) ){
                    pt_read->currentBaseData.v1 = (pt_read->baseCommunication.motorVelRaw[3]<<8+pt_read->baseCommunication.motorVelRaw[4])/1000.0;
                    pt_read->currentBaseData.v2 = (pt_read->baseCommunication.motorVelRaw[5]<<8+pt_read->baseCommunication.motorVelRaw[6])/1000.0;
                    pt_read->currentBaseData.v3 = (pt_read->baseCommunication.motorVelRaw[7]<<8+pt_read->baseCommunication.motorVelRaw[8])/1000.0;
                    pt_read->currentBaseData.v4 = (pt_read->baseCommunication.motorVelRaw[9]<<8+pt_read->baseCommunication.motorVelRaw[10])/1000.0;
                }
            }

            if( (0xfd == pt_read->baseCommunication.batteryRaw[0]) && (0x02 == pt_read->baseCommunication.batteryRaw[1]) &&(0x04 == pt_read->baseCommunication.batteryRaw[2])){
                if( pt_read->CheckRawData(pt_read->baseCommunication.batteryRaw, 5, 4) ){
                    float battery = pt_read->baseCommunication.batteryRaw[3] * 0.1;
                    if(battery < 10.)
                    {
                        ROS_WARN_STREAM("Battery is low, please charge!!!");
                        std::cout << "battery: " <<  battery  << std::endl;
                    }
                }
            }

            if( (0xfd == pt_read->baseCommunication.imuRpyRaw[0]) && (0x0a == pt_read->baseCommunication.imuRpyRaw[1]) &&(0x05 == pt_read->baseCommunication.imuRpyRaw[2])){
                if( pt_read->CheckRawData(pt_read->baseCommunication.imuRpyRaw, 13, 5) ){
                    static bool init_yaw_flag = false;
                    pt_read->currentBaseData.yaw = ((pt_read->baseCommunication.imuRpyRaw[9]>0?-1:1)*(pt_read->baseCommunication.imuRpyRaw[10]*256+pt_read->baseCommunication.imuRpyRaw[11])/100.0)*M_PI/180.0;

                    if(pt_read->currentBaseData.yaw < 0)  pt_read->currentBaseData.yaw += (2*M_PI);
                    if(!init_yaw_flag)
                    {
                        pt_read->currentBaseData.init_yaw = pt_read->currentBaseData.yaw;
                        pt_read->currentBaseData.curr_orien = 0;
                        init_yaw_flag = true;
                    }
                    else
                    {
                        pt_read->currentBaseData.curr_orien = -(pt_read->currentBaseData.yaw - pt_read->currentBaseData.init_yaw);
						if(pt_read->currentBaseData.curr_orien > M_PI) pt_read->currentBaseData.curr_orien -= 2*M_PI;
						if(pt_read->currentBaseData.curr_orien < -M_PI) pt_read->currentBaseData.curr_orien += 2*M_PI; 
                    }
                }
            }

            if( (0xfd == pt_read->baseCommunication.imuRaw[0]) && (0x1f == pt_read->baseCommunication.imuRaw[1]) &&(0x06 == pt_read->baseCommunication.imuRaw[2])){
                if( pt_read->CheckRawData(pt_read->baseCommunication.imuRaw, 34, 6) ){
                    sensor_msgs::Imu imu_data;
                    imu_data.header.stamp = ros::Time::now();
                    imu_data.header.frame_id = pt_read->imu_frame;
                    //angular_velocity
                    imu_data.angular_velocity.x = -1 * ((pt_read->baseCommunication.imuRaw[3]>0?-1:1)*(pt_read->baseCommunication.imuRaw[4]<<8+pt_read->baseCommunication.imuRaw[5]) / 1000.0);
                    imu_data.angular_velocity.y =((pt_read->baseCommunication.imuRaw[6]>0?-1:1)*(pt_read->baseCommunication.imuRaw[7]*256+pt_read->baseCommunication.imuRaw[8]) / 1000.0);
                    imu_data.angular_velocity.z =((pt_read->baseCommunication.imuRaw[9]>0?-1:1)*(pt_read->baseCommunication.imuRaw[10]*256+pt_read->baseCommunication.imuRaw[11]) / 1000.0);
                    imu_data.angular_velocity_covariance  = {1e4, 0, 0, 0, 1e4, 0, 0, 0 , 1e4};
                    //linear_acceleration
                    imu_data.linear_acceleration.x = -1 * ((pt_read->baseCommunication.imuRaw[12]>0?-1:1)*(pt_read->baseCommunication.imuRaw[13]*256+pt_read->baseCommunication.imuRaw[14]) / 1000.0);
                    imu_data.linear_acceleration.y = ((pt_read->baseCommunication.imuRaw[15]>0?-1:1)*(pt_read->baseCommunication.imuRaw[16]*256+pt_read->baseCommunication.imuRaw[17]) / 1000.0);
                    imu_data.linear_acceleration.z =((pt_read->baseCommunication.imuRaw[18]>0?-1:1)*(pt_read->baseCommunication.imuRaw[19]*256+pt_read->baseCommunication.imuRaw[20]) / 1000.0);
                    imu_data.linear_acceleration_covariance = {1e6, 0, 0, 0, 1e6, 0, 0, 0 , 1e6};
                    //orientation
                    imu_data.orientation.w =((pt_read->baseCommunication.imuRaw[21]>0?-1:1)*(pt_read->baseCommunication.imuRaw[22]*256+pt_read->baseCommunication.imuRaw[23]) / 10000.0);
                    imu_data.orientation.x =((pt_read->baseCommunication.imuRaw[24]>0?-1:1)*(pt_read->baseCommunication.imuRaw[25]*256+pt_read->baseCommunication.imuRaw[26]) / 10000.0);
                    imu_data.orientation.y =((pt_read->baseCommunication.imuRaw[27]>0?-1:1)*(pt_read->baseCommunication.imuRaw[28]*256+pt_read->baseCommunication.imuRaw[29]) / 10000.0);
                    imu_data.orientation.z =((pt_read->baseCommunication.imuRaw[30]>0?-1:1)*(pt_read->baseCommunication.imuRaw[31]*256+pt_read->baseCommunication.imuRaw[32]) / 10000.0);
                    imu_data.orientation_covariance = {1e-8, 0, 0, 0, 1e-8, 0, 0, 0 , 1e-8};

                    tf::Quaternion q;
					q.setRPY(0 ,0 ,pt_read->currentBaseData.curr_orien);
					imu_data.orientation.w = q.w();
					imu_data.orientation.x = q.x();
					imu_data.orientation.y = q.y();
					imu_data.orientation.z = q.z();			
					
                    pt_read->imu_pub.publish(imu_data);
                }
            }

            if( (0xfd == pt_read->baseCommunication.carTypeRaw[0]) && (0x02 == pt_read->baseCommunication.carTypeRaw[1]) &&(0x07 == pt_read->baseCommunication.carTypeRaw[2])){
                if( pt_read->CheckRawData(pt_read->baseCommunication.carTypeRaw, 5, 7) ){
                    pt_read->feedback_type = pt_read->baseCommunication.carTypeRaw[3];
                }
            }
            if(pt_read->baseCommunication.recv_update_flag){
                pt_read->myOdomData.update_flag = false;
                if(dt<=0){
                    dt = 1/50.0;
                }
                if(pt_read->myOdomData.oriention >2*M_PI){
                    pt_read->myOdomData.oriention -= 2*M_PI;
                }
                if(pt_read->myOdomData.oriention < -2*M_PI){
                    pt_read->myOdomData.oriention += 2*M_PI;
                }

                double delta_x = 1.061 * dt *(( pt_read->currentBaseData.vx * cos(pt_read->myOdomData.oriention)) - pt_read->currentBaseData.vy * sin(pt_read->myOdomData.oriention));
                double delta_y = 1.077 * dt* ((pt_read->currentBaseData.vx * sin(pt_read->myOdomData.oriention)) + pt_read->currentBaseData.vy * cos(pt_read->myOdomData.oriention));
                pt_read->myOdomData.position_x += delta_x;
                pt_read->myOdomData.position_y += delta_y;
                pt_read->myOdomData.twist_x = 1 *pt_read->currentBaseData.vx;
                pt_read->myOdomData.twist_y = 1*pt_read->currentBaseData.vy;
		pt_read->myOdomData.twist_th = pt_read->currentBaseData.angler_speed;
                pt_read->myOdomData.oriention = pt_read->currentBaseData.curr_orien;

                std::cout << "odom(x,y,yaw): " <<  pt_read->myOdomData.position_x << " " << pt_read->myOdomData.position_y << " " << pt_read->myOdomData.oriention *180.0/M_PI<< std::endl;
                pt_read->baseCommunication.recv_update_flag = 0;
                pt_read->myOdomData.update_flag = true;
            }
        }
    }
}



void CCodbotBase::ReceiveUpdate(int data_len, unsigned char *data_type, unsigned char new_buff) {
    for(int i=0 ;i<data_len-1 ; i++){
        data_type[i] = data_type[i+1];
    }
    data_type[data_len-1]=new_buff;
}

bool CCodbotBase::CheckRawData(unsigned char *raw_data, int data_num, int current_code) {
    unsigned char check_num = 0;
    //exor check
    for(int i = 2;i<(data_num - 1);i++){
        check_num ^= raw_data[i];
    }

    //debug to screen
    if(debug_num>>(current_code-1) & 0x01){
        cout<< "\033[32m#"<<current_code<<" ";
        for (int i = 0; i < data_num; ++i)
        {
            printf("%x ",raw_data[i]);
        }
        std::cout << std::endl;
    }
    //output result
    if(check_num == raw_data[data_num-1]){
        return true;
    }
    else{
        cout<< "\033[31m "<<current_code <<" check up failed!"<<endl;
        return false;
    }
}

void *CCodbotBase::MyWriteThread(void *th) {
    CCodbotBase *pt_write = (CCodbotBase*)th;
    int timer = 0;
    while(true){
        if(pt_write->baseCommunication.send_update_flag){
			 //debug
			if(pt_write->debug_num & 0x01){
				cout<< "\033[32m#1 ";
				for (int i = 0; i < 13; ++i)
		        {
		            printf("%x ", pt_write->baseCommunication.writebuff[i]);
		        }
			}
            std::cout << std::endl;
            pt_write->baseCommunication.nwrite=write(pt_write->baseCommunication.serialNum,pt_write->baseCommunication.writebuff,13);
            
	time_t now = time(0);
	char* dt = ctime(&now);
   	cout << "发送时间：" << dt << endl;
            pt_write->baseCommunication.send_update_flag=0; //clear flag
            timer=0; //clear stop count
        }
       // else if(timer==50) //if not input cmd_vel during 0.5s, stop motor
      //  {
            //stop
           // pt_write->baseCommunication.nwrite=write(pt_write->baseCommunication.serialNum,pt_write->baseCommunication.stopbuff,13);
        //}

        //if(timer>=50)
            //timer=0;
        //else
            //timer++;

        //ros::Duration(0.1).sleep(); //delay 10ms
    }
}
