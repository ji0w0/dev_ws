"""
turtle_cmd_and_pose 노드

turtlesim 거북이(turtle1)의
  1) 받은 '속도 명령'(/turtle1/cmd_vel)과
  2) 실제 '위치/속도 상태'(/turtle1/pose)를
직접 만든 메시지 CmdAndPoseVel 하나에 모아서
1초마다 /cmd_and_pose 토픽으로 발행하는 노드.

   /turtle1/pose    ──► callback_pose ─┐
                                       ├─► self.cmd_pose ──► (1초마다) /cmd_and_pose
   /turtle1/cmd_vel ──► callback_cmd ──┘

실행: ros2 run my_first_package turtle_cmd_and_pose
확인: ros2 topic echo /cmd_and_pose
"""

import rclpy as rp                    # ROS 2 파이썬 라이브러리 (rp 라는 짧은 이름으로 사용)
from rclpy.node import Node           # 모든 노드의 부모 클래스
from turtlesim.msg import Pose        # 거북이 상태 메시지: x, y, theta, linear_velocity, angular_velocity
from geometry_msgs.msg import Twist   # 속도 명령 메시지: linear(x,y,z), angular(x,y,z)
from my_first_package_msgs.msg import CmdAndPoseVel  # 직접 정의한 메시지 (my_first_package_msgs/msg/CmdAndPoseVel.msg)


class CmdAndPose(Node):
    """두 토픽을 구독해 CmdAndPoseVel 하나로 합쳐 발행하는 노드."""

    def __init__(self):
        # 노드 이름 등록 → `ros2 node list` 에 /turtle_cmd_pose 로 표시됨
        super().__init__("turtle_cmd_pose")

        # [구독 1] 거북이의 실제 상태 (turtlesim 이 약 60Hz 로 계속 발행)
        #   create_subscription(메시지 타입, 토픽 이름, 메시지 올 때 호출할 함수, 큐 크기)
        self.sub_pose = self.create_subscription(
            Pose, "/turtle1/pose", self.callback_pose, 10
        )

        # [구독 2] 거북이에게 내려간 속도 명령 (my_publisher, teleop 등이 보낼 때만 들어옴)
        self.sub_cmdvel = self.create_subscription(
            Twist, '/turtle1/cmd_vel', self.callback_cmd, 10
        )

        # [타이머] 1.0초마다 timer_callback 을 자동으로 호출
        self.timer_period = 1.0
        self.timer = self.create_timer(self.timer_period, self.timer_callback)

        # [발행] 합친 결과를 내보낼 퍼블리셔
        #   create_publisher(메시지 타입, 토픽 이름, 큐 크기)
        self.publisher = self.create_publisher(CmdAndPoseVel, "/cmd_and_pose", 10)

        # 데이터를 모아둘 메시지 객체 (처음엔 모든 필드가 0.0)
        # 두 콜백이 각자 맡은 필드를 계속 덮어쓰고, 타이머가 이걸 그대로 발행한다.
        self.cmd_pose = CmdAndPoseVel()

    def callback_pose(self, msg):
        """/turtle1/pose 메시지가 올 때마다 호출 → 실제 위치·속도 저장."""
        self.cmd_pose.pose_x = msg.x                         # 현재 x 좌표
        self.cmd_pose.pose_y = msg.y                         # 현재 y 좌표
        self.cmd_pose.linear_vel = msg.linear_velocity       # 실제 직진 속도
        self.cmd_pose.angular_vel = msg.angular_velocity     # 실제 회전 속도

    def callback_cmd(self, msg):
        """/turtle1/cmd_vel 메시지가 올 때마다 호출 → 명령 속도 저장 후 출력."""
        self.cmd_pose.cmd_vel_linear = msg.linear.x          # 명령된 직진 속도 (앞/뒤)
        self.cmd_pose.cmd_vel_angular = msg.angular.z        # 명령된 회전 속도 (좌/우)
        print(self.cmd_pose)                                 # 명령이 들어온 시점의 전체 값 확인용 출력

    def timer_callback(self):
        """1초마다 호출 → 지금까지 모인 최신 값을 /cmd_and_pose 로 발행."""
        self.publisher.publish(self.cmd_pose)


def main(args=None):
    rp.init(args=args)                       # 1. ROS 2 통신 초기화

    turtle_cmd_pose_node = CmdAndPose()      # 2. 노드 생성 (__init__ 실행 → 구독/타이머/발행 준비)
    rp.spin(turtle_cmd_pose_node)            # 3. 노드를 계속 살려두며 콜백 실행 (Ctrl+C 전까지 여기서 대기)

    turtle_cmd_pose_node.destroy_node()      # 4. 종료 시 노드 정리
    rp.shutdown()                            # 5. ROS 2 통신 종료


if __name__ == "__main__":
    main()
