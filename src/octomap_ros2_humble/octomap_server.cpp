#include <octomap_ros2_humble/octomap_ros2_humble/octomap_server.hpp>
#include <octomap_ros2_humble/conversions.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

OctomapServer::OctomapServer() : Node("octomap_server") {
    this->declare_parameter("resolution", 0.1);
    this->declare_parameter("max_range", 10.0);
    this->declare_parameter("max_nodes", 100000);
    this->declare_parameter("publish_free_space", false);

    resolution_ = this->get_parameter("resolution").as_double();
    max_range_ = this->get_parameter("max_range").as_double();
    auto max_nodes = this->get_parameter("max_nodes").as_int();
    auto publish_free_space = this->get_parameter("publish_free_space").as_bool();

    octomap_ = std::make_shared<octomap::OcFoM>(resolution_);
    octomap_->maxNodes(max_nodes);
    octomap_->setOccupancyThreshold(0.7);

    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    point_cloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
        "/cloud_in", 10, std::bind(&OctomapServer::cloud_callback, this, std::placeholders::_1));

    map_pub_ = this->create_publisher<octomap_msgs::msg::Octomap>("/octomap_binary", 10);

    RCLCPP_INFO(this->get_logger(), "Octomap Server initialized");
    RCLCPP_INFO(this->get_logger(), "  Resolution: %.3f m", resolution_);
    RCLCPP_INFO(this->get_logger(), "  Max range: %.1f m", max_range_);
    RCLCPP_INFO(this->get_logger(), "  Max nodes: %d", max_nodes);
}

OctomapServer::~OctomapServer() {}

void OctomapServer::cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
    // Transform lookup with proper error handling
    geometry_msgs::msg::TransformStamped transform;
    try {
        // Wait up to 1 second for transform
        transform = tf_buffer_->lookupTransform(
            "world", 
            msg->header.frame_id, 
            rclcpp::Time(), 
            rclcpp::Duration::from_seconds(1.0)
        );
    } catch (const tf2::TransformException& ex) {
        RCLCPP_WARN_ONCE(this->get_logger(), "TF lookup failed: %s", ex.what());
        return;
    }

    // Convert pointcloud using our utility function
    octomap::Pointcloud points;
    try {
        octomap_ros2_humble::pointCloud2ToOctomap(*msg, points);
    } catch (const std::runtime_error& e) {
        RCLCPP_ERROR(this->get_logger(), "Pointcloud conversion failed: %s", e.what());
        return;
    }

    if (points.empty()) {
        RCLCPP_DEBUG(this->get_logger(), "No valid points in pointcloud");
        return;
    }

    // Convert TF transform to tf2::Transform
    tf2::Transform tf_transform;
    tf2::fromMsg(transform.transform, tf_transform);

    // Process points
    int count = 0;
    for (const auto& pt : points) {
        // Transform point to world frame
        tf2::Vector3 point_sensor(pt.x(), pt.y(), pt.z());
        tf2::Vector3 point_world = tf_transform * point_sensor;

        // Distance check from origin
        double dist = std::sqrt(
            point_world.x()*point_world.x() + 
            point_world.y()*point_world.y() + 
            point_world.z()*point_world.z()
        );
        if (dist > max_range_) {
            continue;
        }

        // Insert point into octomap
        octomap_->insertNode(
            point_world.x(), 
            point_world.y(), 
            point_world.z()
        );
        count++;
    }

    if (count > 0) {
        RCLCPP_DEBUG(this->get_logger(), "Inserted %d points into Octomap", count);
        publish_map();
    }
}

void OctomapServer::publish_map() {
    auto msg = std::make_shared<octomap_msgs::msg::Octomap>();
    
    // Set header
    msg->header.stamp = this->get_clock()->now();
    msg->header.frame_id = "world";
    
    // Serialize octomap
    std::stringstream ss;
    octomap_->write(ss);
    std::string data = ss.str();
    msg->data.assign(data.begin(), data.end());
    
    // Publish
    map_pub_->publish(*msg);
    
    RCLCPP_DEBUG(this->get_logger(), "Published Octomap with %u bytes", (unsigned int)msg->data.size());
}
