#include <octomap_ros2_humble/octomap_ros2_humble/octomap_server.hpp>

OctomapServer::OctomapServer() : Node("octomap_server") {
    this->declare_parameter("resolution", 0.1);
    this->declare_parameter("max_range", 10.0);

    resolution_ = this->get_parameter("resolution").as_double();
    max_range_ = this->get_parameter("max_range").as_double();

    octomap_ = std::make_shared<octomap::OcFoM>(resolution_);
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    point_cloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
        "/cloud_in", 10, std::bind(&OctomapServer::cloud_callback, this, std::placeholders::_1));

    map_pub_ = this->create_publisher<octomap_msgs::msg::Octomap>("/octomap_binary", 10);

    RCLCPP_INFO(this->get_logger(), "Octomap Server initialized. Resolution: %f", resolution_);
}

OctomapServer::~OctomapServer() {}

void OctomapServer::cloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
    geometry_msgs::msg::TransformStamped transform;
    try {
        transform = tf_buffer_->lookupTransform("world", msg->header.frame_id, tf2::Time(0));
    } catch (tf2::TransformException &ex) {
        RCLCPP_WARN(this->get_logger(), "TF lookup failed: %s", ex.what());
        return;
    }

    sensor_msgs::PointCloud2Iterator<float> iter_x(msg->header.frame_id, msg->points, sensor_msgs::PointCloud2Iterator<float>::FieldX);
    sensor_msgs::PointCloud2Iterator<float> iter_y(msg->header.frame_id, msg->points, sensor_msgs::PointCloud2Iterator<float>::FieldY);
    sensor_msgs::PointCloud2Iterator<float> iter_z(msg->header.frame_id, msg->points, sensor_msgs::PointCloud2Iterator<float>::FieldZ);

    int count = 0;
    for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z) {
        float x = *iter_x;
        float y = *iter_y;
        float z = *iter_z;

        if (std::isnan(x) || std::isnan(y) || std::isnan(z)) continue;

        // Transform point to world frame
        tf2::Vector3 point_sensor(x, y, z);
        tf2::Transform transform_tf;
        tf2::geometry_msgs::tf2::transformStampedTotf2(transform, transform_tf);
        tf2::Vector3 point_world = transform_tf * point_sensor;

        // Distance check
        float dist = std::sqrt(point_world.x()*point_world.x() + point_world.y()*point_world.y() + point_world.z()*point_world.z());
        if (dist > max_range_) continue;

        octomap_->insertNode(point_world.x(), point_world.y(), point_world.z());
        count++;
    }

    RCLCPP_DEBUG(this->get_logger(), "Inserted %d points into Octomap", count);
    publish_map();
}

void OctomapServer::publish_map() {
    auto msg = octomap_msgs::msg::Octomap();
    std::stringstream ss;
    octomap_->write(ss);
    std::string data = ss.str();
    msg.data.assign(data.begin(), data.end());
    map_pub_->publish(msg);
}
