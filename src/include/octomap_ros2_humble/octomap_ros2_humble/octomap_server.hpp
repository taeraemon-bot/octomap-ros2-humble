#ifndef OCTOMAP_ROS2_HUMBLE_OCTOMAP_SERVER_HPP
#define OCTOMAP_ROS2_HUMBLE_OCTOMAP_SERVER_HPP

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <octomap_msgs/msg/octomap.hpp>
#include <octomap/octomap.h>
#include <octomap/OcFoM.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

class OctomapServer : public rclcpp::Node {
public:
    OctomapServer();
    ~OctomapServer();

    void cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg);
    void publish_map();

private:
    double resolution_;
    double max_range_;
    std::shared_ptr<octomap::OcFoM> octomap_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr point_cloud_sub_;
    rclcpp::Publisher<octomap_msgs::msg::Octomap>::SharedPtr map_pub_;
};

#endif // OCTOMAP_ROS2_HUMBLE_OCTOMAP_SERVER_HPP
