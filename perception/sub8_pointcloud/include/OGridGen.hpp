#pragma once
#include <geometry_msgs/Point.h>
#include <mil_blueview_driver/BlueViewPing.h>
#include <nav_msgs/OccupancyGrid.h>
#include <ros/ros.h>

#include <tf/transform_listener.h>
#include <tf2/convert.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_msgs/TFMessage.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_sensor_msgs/tf2_sensor_msgs.h>

#include <opencv2/core/core.hpp>
#include <stdexcept>
#include "opencv2/opencv.hpp"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>

#include <boost/circular_buffer.hpp>

#include <sub8_msgs/Bounds.h>

#include <waypoint_validity.hpp>

#include <Classification.hpp>

#include <mil_msgs/RangeStamped.h>
#include <std_srvs/Trigger.h>
#include <ros_alarms/listener.hpp>

class OGridGen

{
public:
  OGridGen();
  void publish_big_pointcloud(const ros::TimerEvent &);

  void callback(const mil_blueview_driver::BlueViewPingPtr &ping_msg);
  void dvl_callback(const mil_msgs::RangeStampedConstPtr &dvl);

  bool clear_ogrid_callback(std_srvs::Trigger::Request &req, std_srvs::Trigger::Response &res);

  // Publish mat_ogrid
  void publish_ogrid();
  // Project point_cloud and make a persistant ogrid
  void process_persistant_ogrid(pcl::PointCloud<pcl::PointXYZI>::Ptr point_cloud_plane);
  // Convert persistant ogrid to a mat_ogrid
  void populate_mat_ogrid();

private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_to_imaging_sonar_;
  ros::Subscriber sub_to_dvl_;

  tf::TransformListener listener_;

  ros_alarms::AlarmListener<> kill_listener_;
  // remember if the sub was killed or not in order to reset ogrid origin
  bool was_killed_;
  // where the ogrid is with respect to map frame
  cv::Point mat_origin_;

  // Publish ogrid and pointclouds
  ros::Publisher pub_grid_;
  ros::Publisher pub_point_cloud_filtered_;
  ros::Publisher pub_point_cloud_raw_;
  ros::Publisher pub_point_cloud_plane_;

  ros::ServiceServer clear_ogrid_service_;
  ros::Timer timer_;

  // A CV_32F Mat to store probability of occupied/unoccupied spaces
  cv::Mat persistant_ogrid_;
  float hit_prob_;

  cv::Mat mat_ogrid_;
  float ogrid_size_;
  float resolution_;
  double dvl_range_;
  int min_intensity_;

  ros::ServiceClient service_get_bounds_;
  tf::StampedTransform transform_;

  // Storage container for the pointcloud
  boost::circular_buffer<pcl::PointXYZI> point_cloud_buffer_;

  std::vector<cv::Point> bounds_;

  Classification classification_;
};
