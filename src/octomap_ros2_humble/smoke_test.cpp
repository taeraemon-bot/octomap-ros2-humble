#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <geometry_msgs/msg/transform_stamp.hpp>

class SmokeTestNode : public rclcpp::Node {
public:
    SmokeTestNode() : Node("octomap_smoke_test") {
        pub_cloud_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_in", 10);
        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(1000), std::bind(&SmokeTestNode::send_data, this));
        
        RCLCPP_INFO(this->get_logger(), "Smoke Test Node started. Sending dummy cloud and TF...");
    }

    void send_data() {
        // 1. Broadcast TF: world -> sensor
        geometry_msgs::msg::TransformStamped t;
        t.header.stamp = this->get_clock()->now();
        t.header.frame_id = "world";
        t.child_frame_id = "sensor_frame";
        t.transform.translation.x = 1.0;
        t.transform.translation.y = 1.0;
        t.transform.translation.z = 1.0;
        t.transform.rotation.w = 1.0;
        broadcaster_->sendTransform(t);

        // 2. Send dummy point cloud
        auto cloud = sensor_msgs::msg::PointCloud2();
        cloud.header.stamp = this->get_clock()->now();
        cloud.header.frame_id = "sensor_frame";
        
        // Define a single point at (0,0,0) in sensor frame -> (1,1,1) in world frame
        cloud.height = 1;
        cloud.width = 1;
        cloud.fields.push_back(sensor_msgs::PointField());
        cloud.fields[0].name = "x";
        cloud.fields[0].offset = 0;
        cloud.fields[0].datatype = sensor_msgs::PointField::FLOAT32;
        cloud.fields[0].count = 1;

        cloud.fields.push_back(sensor_msgs::PointField());
        cloud.fields[1].name = "y";
        cloud.fields[1].offset = 4;
        cloud.fields[1].datatype = sensor_msgs::PointField::FLOAT32;
        cloud.fields[1].count = 1;

        cloud.fields.push_back(sensor_msgs::PointField());
        cloud.fields[2].name = "z";
        cloud.fields[2].offset = 8;
        cloud.fields[2].datatype = sensor_msgs::PointField::FLOAT32;
        cloud.fields[2].count = 1;

        cloud.point_step = 12;
        cloud.row_step = 12;
        cloud.data.resize(12);
        
        float p[3] = {0.0f, 0.0f, 0.0f};
        std::memcpy(cloud.data.data(), p, 12);

        pub_cloud_->publish(cloud);
        RCLCPP_INFO(this->get_logger(), "Sent dummy point at (1,1,1) world");
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_cloud_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SmokeTestNode>());
    rclcpp::shutdown();
    return 0;
}
