#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <octomap_msgs/msg/octomap.hpp>
#include <octomap/octomap.h>
#include <octomap/OcFoM.h>
#include <//TF2/tf2_ros/transform_listener.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

class OctomapServer : public rclcpp::Node {
public:
    OctomapServer() : Node("octomap_server") {
        // Parameter initialization
        this->declare_parameter("resolution", 0.1);
        this->declare_parameter("max_range", 10.0);

        resolution_ = this->get_parameter("resolution").as_double();
        max_range_ = this->get_parameter("max_range").as_double();

        // Initialize Octomap
        octomap_ = std::make_shared<octomap::OcFoM>(resolution_);

        // TF Buffer and Listener
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        // Subscribers
        point_cloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "/cloud_in", 10, std::bind(&OctomapServer::cloud_callback, this, std::placeholders::_1));

        // Publisher
        map_pub_ = this->create_publisher<octomap_msgs::msg::Octomap>("/octomap_binary", 10);

        RCLCPP_INFO(this->get_logger(), "Octomap Server Node initialized with resolution: %f", resolution_);
    }

    void cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
        // 1. Get transform from world to sensor
        geometry_msgs::msg::TransformStamped transform;
        try {
            transform = tf_buffer_->lookupTransform("world", msg->header.frame_id, tf2::Time(0));
        } catch (tf2::TransformException &ex) {
            RCLCPP_WARN(this->get_logger(), "Could not transform %s to world: %s", msg->header.frame_id.c_str(), ex.what());
            return;
        }

        // 2. Iterate through points and insert into Octomap
        sensor_msgs::PointCloud2Iterator<float> iter(msg->header.frame_id, msg->points, 
                                                     sensor_msgs::PointCloud2Iterator<float>::// la- la- la
                                                     // Need to iterate over X, Y, Z
                                                     // For simplicity in the skeleton, we'll use a loop here
                                                     // (Actual implementation will be more robust)
                                                     );

        // This is a simplified insertion for the skeleton
        // Real logic will go here in the next commit
        RCLCPP_DEBUG(this->get_logger(), "Received point cloud, updating map...");
        
        // Trigger map publishing periodically or on demand
        publish_map();
    }

    void publish_map() {
        auto msg = octomap_msgs::msg::Octomap();
        // Binary serialization of the Octomap
        std::stringstream ss;
        octomap_->write(ss);
        std::string data = ss.str();
        
        msg.data.assign(data.begin(), data.end());
        map_pub_->publish(msg);
    }

private:
    double resolution_;
    double max_range_;
    std::shared_ptr<octomap::OcFoM> octomap_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr point_cloud_sub_;
    rclcpp::Publisher<octomap_msgs::msg::Octomap>::SharedPtr map_pub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OctomapServer>());
    rclcpp::shutdown();
    return 0;
}
