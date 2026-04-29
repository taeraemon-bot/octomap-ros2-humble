#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <geometry_msgs/msg/transform_stamp.hpp>
#include <chrono>
#include <memory>
#include <vector>
#include <cmath>

class SmokeTestNode : public rclcpp::Node {
public:
    SmokeTestNode() : Node("octomap_smoke_test"), message_count_(0) {
        // Parameters
        this->declare_parameter("points_per_cloud", 10);
        this->declare_parameter("number_of_messages", 5);
        this->declare_parameter("publish_frequency", 1.0);
        this->declare_parameter("random_seed", 42);
        
        points_per_cloud_ = this->get_parameter("points_per_cloud").as_int();
        num_messages_ = this->get_parameter("number_of_messages").as_int();
        publish_freq_ = this->get_parameter("publish_frequency").as_double();
        random_seed_ = this->get_parameter("random_seed").as_int();
        
        // Publishers
        pub_cloud_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_in", 10);
        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        
        // Timer
        auto period_ms = static_cast<int64_t>(1000.0 / publish_freq_);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(period_ms), 
            std::bind(&SmokeTestNode::publish_data, this)
        );
        
        RCLCPP_INFO(this->get_logger(), "=== OctoMap Smoke Test Node ===");
        RCLCPP_INFO(this->get_logger(), "Configuration:");
        RCLCPP_INFO(this->get_logger(), "  Points per cloud: %d", points_per_cloud_);
        RCLCPP_INFO(this->get_logger(), "  Number of messages: %s", 
                   num_messages_ < 0 ? "unlimited" : std::to_string(num_messages_).c_str());
        RCLCPP_INFO(this->get_logger(), "  Publish frequency: %.1f Hz", publish_freq_);
        RCLCPP_INFO(this->get_logger(), "  Random seed: %d", random_seed_);
        RCLCPP_INFO(this->get_logger(), "Publishing dummy point clouds and TF transforms...");
    }

    void publish_data() {
        message_count_++;
        
        // Check if we should stop
        if (num_messages_ > 0 && message_count_ >= num_messages_) {
            RCLCPP_INFO(this->get_logger(), "Published %d messages. Test complete.", message_count_);
            rclcpp::shutdown();
            return;
        }

        // 1. Broadcast TF: world -> sensor_frame
        publish_transform();
        
        // 2. Publish point cloud with varied points
        publish_point_cloud();
        
        RCLCPP_INFO(this->get_logger(), "Message #%d published", message_count_);
    }

private:
    void publish_transform() {
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = this->get_clock()->now();
        transform.header.frame_id = "world";
        transform.child_frame_id = "sensor_frame";
        
        // Fixed transform: sensor at (1, 1, 1) in world frame
        transform.transform.translation.x = 1.0;
        transform.transform.translation.y = 1.0;
        transform.transform.translation.z = 1.0;
        transform.transform.rotation.w = 1.0;  // No rotation
        
        broadcaster_->sendTransform(transform);
    }

    void publish_point_cloud() {
        auto cloud = create_point_cloud();
        pub_cloud_->publish(std::move(cloud));
    }

    sensor_msgs::msg::PointCloud2 create_point_cloud() {
        sensor_msgs::msg::PointCloud2 cloud;
        
        // Header
        cloud.header.stamp = this->get_clock()->now();
        cloud.header.frame_id = "sensor_frame";
        
        // Point cloud structure (XYZ only)
        cloud.fields.clear();
        add_point_field(cloud, "x", 0, sensor_msgs::msg::PointField::FLOAT32);
        add_point_field(cloud, "y", 4, sensor_msgs::msg::PointField::FLOAT32);
        add_point_field(cloud, "z", 8, sensor_msgs::msg::PointField::FLOAT32);
        
        cloud.point_step = 12;  // 3 floats * 4 bytes
        cloud.row_step = 0;  // Dense point cloud
        cloud.height = 1;  // Point cloud (not organized)
        cloud.width = static_cast<uint32_t>(points_per_cloud_);
        cloud.is_bigendian = false;
        cloud.is_dense = true;
        
        // Allocate memory
        cloud.data.resize(cloud.width * cloud.point_step);
        
        // Fill with points
        fill_point_cloud_data(cloud, message_count_);
        
        return cloud;
    }

    void add_point_field(sensor_msgs::msg::PointCloud2& cloud, 
                        const std::string& name, 
                        uint32_t offset, 
                        uint8_t datatype) {
        sensor_msgs::msg::PointField field;
        field.name = name;
        field.offset = offset;
        field.datatype = datatype;
        field.count = 1;
        cloud.fields.push_back(field);
    }

    void fill_point_cloud_data(sensor_msgs::msg::PointCloud2& cloud, int message_num) {
        // Create a pattern of points
        // Base pattern: points distributed in a small area around origin
        float spacing = 0.5f;
        int count = 0;
        
        for (uint32_t i = 0; i < cloud.width && count < points_per_cloud_; ++i) {
            // Create a grid pattern with some variation based on message number
            float x = static_cast<float>((i % 3) * spacing - spacing);
            float y = static_cast<float>((i / 3 % 3) * spacing - spacing);
            float z = static_cast<float>((i / 9) * spacing);
            
            // Add some variation based on message number
            x += static_cast<float>(message_num * 0.1);
            
            // Write to cloud data
            size_t idx = i * cloud.point_step;
            reinterpret_cast<float*>(&cloud.data[idx])[0] = x;  // x
            reinterpret_cast<float*>(&cloud.data[idx])[1] = y;  // y
            reinterpret_cast<float*>(&cloud.data[idx])[2] = z;  // z
            
            count++;
        }
        
        // Adjust width to actual number of points
        cloud.width = static_cast<uint32_t>(count);
        cloud.data.resize(count * cloud.point_step);
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_cloud_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
    rclcpp::TimerBase::SharedPtr timer_;
    
    int points_per_cloud_;
    int num_messages_;
    double publish_freq_;
    int random_seed_;
    int message_count_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SmokeTestNode>());
    rclcpp::shutdown();
    return 0;
}
