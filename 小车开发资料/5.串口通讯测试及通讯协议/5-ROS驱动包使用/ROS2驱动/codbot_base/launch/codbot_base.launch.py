from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
   return LaunchDescription([
      Node(
         package='codbot_base',
         node_namespace='codbot_base',
         node_executable='codbot_base',
         node_name='codbot',
         output='screen',
         parameters=[{"debug_num": 3},
                     {"baud_rate": "B115200"},
                     {"cmd_port": "/dev/base"},
                     {"coeff_odom_x": 1.131},
                     {"coeff_odom_y": 1.077},
                     {"cmd_vel_linear_max": 0.3},
                     {"cmd_vel_linear_min": 0.1},
                     {"cmd_vel_anglar_max": 2.0},
                     {"cmd_vel_anglar_min": 0.0},
                     {"cmd_vel_angle_max": 25.0},
                     {"cmd_vel_angle_min": 1.0},
                     {"current_type": "M100"},
                     {"cmd_vel_topic": "/cmd_vel"},
                     {"odom_topic": "/odom"},
                     {"imu_topic": "/imu"},
                     {"wheel_left_speed_pub_topic": "wheel_left_speed"},
                     {"wheel_right_speed_pub_topic": "wheel_right_speed"},
                     {"imu_frame": "imu_link"},
                     {"odom_frame": "odom"},
                     {"odom_child_frame": "base_footprint"}]
      )
   ])
