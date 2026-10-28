#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class TurtlesimPublisher : public rclcpp::Node
{
public:
  TurtlesimPublisher() : Node("turtlesim_publisher")
  {
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
    timer_ = this->create_wall_timer(
      500ms, std::bind(&TurtlesimPublisher::timer_callback, this));
  }

private:
  void timer_callback()
  {
    geometry_msgs::msg::Twist msg;
    msg.linear.x = 2.0;
    msg.angular.z = 2.0;
    publisher_->publish(msg);
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TurtlesimPublisher>());
  rclcpp::shutdown();
  return 0;
}