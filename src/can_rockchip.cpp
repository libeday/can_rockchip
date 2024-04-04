#include <ros/ros.h>
#include <socketcan_bridge/socketcan_to_topic.h>
#include <socketcan_bridge/topic_to_socketcan.h>
#include <can_rockchip/CreateMsg.h>

const uint32_t PDO_ID = 180;
const uint32_t NODE_ID = 0x27;

ros::Publisher frame_pub;
can_msgs::Frame output;
boost::array<uint8_t, 8> arr;

bool create_frame_from_msg(can_rockchip::CreateMsg::Request &req, can_rockchip::CreateMsg::Response &res) {
    
    output.data = req.data;
    output.dlc = req.dlc;
    output.id = req.id;

    output.is_error = req.is_error;
    output.is_extended = req.is_extended;
    output.is_rtr = req.is_rtr;
    res.success = true;
    return true;
}

can_msgs::Frame make_frame() {

    boost::array<uint8_t, 8> arr;
    arr.assign(0);
    arr[0] = 0x2F;
    arr[1] = 0x27;
    arr[2] = 0x60;
    arr[3] = 0x01;
    arr[4] = 0x01;
    output.header.stamp = ros::Time::now();

    output.data = arr;
    output.dlc = 8;
    output.id = 0x627;
    output.is_error = false;
    output.is_extended = false;
    output.is_rtr = false;

    return output;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "can_rockchip");
    ros::NodeHandle nh;
    printf("INIT!\n");

    ros::ServiceServer set_coeff_srv = nh.advertiseService("can_rockchip/CreateMsg", create_frame_from_msg);
    frame_pub = nh.advertise<can_msgs::Frame>("sent_messages", 1000);
    ros::Rate loop_rate(1);

    output = make_frame();  

    while(ros::ok()){
        frame_pub.publish(output);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}