#include <rclcpp/rclcpp.hpp>
#include <octomap_msgs/msg/octomap.hpp>
#include <octomap/OcTree.h>
#include <octomap/OcTreeKey.h>
#include <iostream>
#include <sstream>
#include <memory>

class OctomapClient : public rclcpp::Node {
public:
    OctomapClient() : Node("octomap_client") {
        this->declare_parameter("visualize_statistics", true);
        this->declare_parameter("max_updates", -1);  // -1 = unlimited

        auto viz_stats = this->get_parameter("visualize_statistics").as_bool();
        max_updates_ = this->get_parameter("max_updates").as_int();

        sub_ = this->create_subscription<octomap_msgs::msg::Octomap>(
            "/octomap_binary", 
            10, 
            std::bind(&OctomapClient::map_callback, this, std::placeholders::_1),
            rclcpp::SensorDataQoS().keepLast(1)
        );
        
        received_count_ = 0;
        
        RCLCPP_INFO(this->get_logger(), "Octomap Client initialized");
        RCLCPP_INFO(this->get_logger(), "  Visualize statistics: %s", viz_stats ? "yes" : "no");
        RCLCPP_INFO(this->get_logger(), "  Max updates: %s", max_updates_ < 0 ? "unlimited" : std::to_string(max_updates_).c_str());
        RCLCPP_INFO(this->get_logger(), "Waiting for map on /octomap_binary...");
    }

    void map_callback(const octomap_msgs::msg::Octomap::SharedPtr msg) {
        received_count_++;
        
        // Check if we should stop processing
        if (max_updates_ > 0 && received_count_ >= max_updates_) {
            RCLCPP_INFO(this->get_logger(), "Reached max updates limit (%d), shutting down", max_updates_);
            rclcpp::shutdown();
            return;
        }

        try {
            // Deserialize the octomap
            std::stringstream ss;
            ss.write(msg->data.data(), msg->data.size());
            
            auto tree = std::unique_ptr<octomap::OcTree>(octomap::readBinary(ss));
            
            if (!tree) {
                RCLCPP_ERROR(this->get_logger(), "Failed to deserialize Octomap (empty or corrupted data)");
                return;
            }

            // Basic statistics
            RCLCPP_INFO(this->get_logger(), "=== Received Octomap #%d ===", received_count_);
            RCLCPP_INFO(this->get_logger(), "  Resolution: %.4f m", tree->getResolution());
            RCLCPP_INFO(this->get_logger(), "  Tree depth: %d", tree->getTreeDepth());
            RCLCPP_INFO(this->get_logger(), "  Total nodes: %zu", tree->size());
            RCLCPP_INFO(this->get_logger(), "  Occupied nodes: %zu", tree->getOccupiedCount());
            RCLCPP_INFO(this->get_logger(), "  Memory usage: %.2f MB", tree->memSize() / (1024.0 * 1024.0));

            // Bounds information
            octomap::point3d min_bbox, max_bbox;
            tree->calculateBounds(min_bbox, max_bbox);
            RCLCPP_INFO(this->get_logger(), "  Bounds: [%.2f, %.2f, %.2f] to [%.2f, %.2f, %.2f]",
                       min_bbox.x(), min_bbox.y(), min_bbox.z(),
                       max_bbox.x(), max_bbox.y(), max_bbox.z());

            // Occupancy rate
            if (tree->size() > 0) {
                double occupancy_rate = static_cast<double>(tree->getOccupiedCount()) / tree->size() * 100.0;
                RCLCPP_INFO(this->get_logger(), "  Occupancy rate: %.2f%%", occupancy_rate);
            }

            // Header info
            RCLCPP_DEBUG(this->get_logger(), "  Frame ID: %s", msg->header.frame_id.c_str());
            RCLCPP_DEBUG(this->get_logger(), "  Timestamp: %f", msg->header.stamp.sec + msg->header.stamp.nanosec / 1e9);

        } catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "Error processing Octomap: %s", e.what());
        }
    }

private:
    rclcpp::Subscription<octomap_msgs::msg::Octomap>::SharedPtr sub_;
    int received_count_;
    int max_updates_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OctomapClient>());
    rclcpp::shutdown();
    return 0;
}
