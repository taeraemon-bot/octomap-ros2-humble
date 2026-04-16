#include <rclcpp/rclcpp.hpp>
#include <octomap_msgs/msg/octomap.hpp>
#include <octomap/octomap.h>
#include <iostream>
#include <sstream>

class OctomapClient : public rclcpp::Node {
public:
    OctomapClient() : Node("octomap_client") {
        sub_ = this->create_subscription<octomap_msgs::msg::Octomap>(
            "/octomap_binary", 10, std::bind(&OctomapClient::map_callback, this, std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "Octomap Client initialized. Waiting for map...");
    }

    void map_callback(const octomap_msgs::msg::Octomap::SharedPtr msg) {
        std::stringstream ss;
        ss.write(msg->data.data(), msg->data.size());
        
        octomap::OcTree* tree = octomap::readBinary(ss);
        if (tree) {
            RCLCPP_INFO(this->get_logger(), "Received Octomap! Nodes: %zu", tree->size());
            delete tree;
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to deserialize Octomap");
        }
    }

private:
    rclcpp::Subscription<octomap_msgs::msg::Octomap>::SharedPtr sub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OctomapClient>());
    rclcpp::shutdown();
    return 0;
}
