/**
 * OctoMap ROS 2 Humble Integration - Conversion Utilities
 * 
 * Based on original octomap_ros by A. Hornung, University of Freiburg
 * Ported to ROS 2 Humble with modern C++ practices
 * 
 * @author Mashiro (Digital Maid Assistant)
 * @license BSD
 */

#ifndef OCTOMAP_ROS2_HUMBLE_CONVERSIONS_H
#define OCTOMAP_ROS2_HUMBLE_CONVERSIONS_H

#include <octomap/octomap.h>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Vector3.h>
#include <tf2/LinearMath/Transform.h>
#include <tf2_ros/transform_datatypes.h>

namespace octomap_ros2_humble {

/**
 * @brief Convert octomap::point3d_list to sensor_msgs::PointCloud2
 * 
 * @param points - List of OctoMap 3D points
 * @param cloud - Output PointCloud2 message
 */
void pointsOctomapToPointCloud2(const octomap::point3d_list& points, 
                                 sensor_msgs::msg::PointCloud2& cloud);

/**
 * @brief Convert sensor_msgs::PointCloud2 to octomap::Pointcloud
 * 
 * @param cloud - Input PointCloud2 message
 * @param octomapCloud - Output OctoMap pointcloud
 */
void pointCloud2ToOctomap(const sensor_msgs::msg::PointCloud2& cloud,
                          octomap::Pointcloud& octomapCloud);

/**
 * @brief Convert octomap::point3d to geometry_msgs::Point
 */
inline geometry_msgs::msg::Point pointOctomapToMsg(const octomap::point3d& pt) {
    geometry_msgs::msg::Point msg_pt;
    msg_pt.x = pt.x();
    msg_pt.y = pt.y();
    msg_pt.z = pt.z();
    return msg_pt;
}

/**
 * @brief Convert geometry_msgs::Point to octomap::point3d
 */
inline octomap::point3d pointMsgToOctomap(const geometry_msgs::msg::Point& pt_msg) {
    return octomap::point3d(pt_msg.x, pt_msg.y, pt_msg.z);
}

/**
 * @brief Convert octomap::point3d to tf2::Vector3
 */
inline tf2::Vector3 pointOctomapToTf(const octomap::point3d& pt) {
    return tf2::Vector3(pt.x(), pt.y(), pt.z());
}

/**
 * @brief Convert tf2::Vector3 to octomap::point3d
 */
inline octomap::point3d pointTfToOctomap(const tf2::Vector3& pt_tf) {
    return octomap::point3d(pt_tf.x(), pt_tf.y(), pt_tf.z());
}

/**
 * @brief Convert octomath::Quaternion to tf2::Quaternion
 */
inline tf2::Quaternion quaternionOctomapToTf(const octomath::Quaternion& q) {
    return tf2::Quaternion(q.x(), q.y(), q.z(), q.w());
}

/**
 * @brief Convert tf2::Quaternion to octomath::Quaternion
 */
inline octomath::Quaternion quaternionTfToOctomap(const tf2::Quaternion& q_tf) {
    return octomath::Quaternion(q_tf.w(), q_tf.x(), q_tf.y(), q_tf.z());
}

/**
 * @brief Convert geometry_msgs::Quaternion to octomath::Quaternion
 */
inline octomath::Quaternion quaternionMsgToOctomap(const geometry_msgs::msg::Quaternion& q_msg) {
    return octomath::Quaternion(q_msg.w, q_msg.x, q_msg.y, q_msg.z);
}

/**
 * @brief Convert octomath::Quaternion to geometry_msgs::Quaternion
 */
inline geometry_msgs::msg::Quaternion quaternionOctomapToMsg(const octomath::Quaternion& q) {
    geometry_msgs::msg::Quaternion msg_q;
    msg_q.w = q.w();
    msg_q.x = q.x();
    msg_q.y = q.y();
    msg_q.z = q.z();
    return msg_q;
}

/**
 * @brief Convert octomap::pose6d to tf2::Transform
 */
inline tf2::Transform poseOctomapToTf(const octomap::pose6d& pose) {
    tf2::Transform tf_pose;
    tf_pose.setOrigin(pointOctomapToTf(octomap::point3d(pose.tx(), pose.ty(), pose.tz())));
    tf_pose.setRotation(quaternionOctomapToTf(pose.rot()));
    return tf_pose;
}

/**
 * @brief Convert tf2::Transform to octomap::pose6d
 */
inline octomap::pose6d poseTfToOctomap(const tf2::Transform& tf_pose) {
    return octomap::pose6d(
        pointTfToOctomap(tf_pose.getOrigin()),
        quaternionTfToOctomap(tf_pose.getRotation())
    );
}

} // namespace octomap_ros2_humble

#endif // OCTOMAP_ROS2_HUMBLE_CONVERSIONS_H
