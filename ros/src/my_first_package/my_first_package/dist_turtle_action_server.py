"""
dist_turtle_action_server 노드

'정해진 속도로, 정해진 거리만큼 거북이를 움직이는' 액션 서버.

  클라이언트 (예: ros2 action send_goal ...)
      │  Goal     : linear_x, angular_z, dist     (이 속도로 dist 만큼 가라)
      ▼
  [DistTurtleServer]  ── /turtle1/cmd_vel 발행 ──► turtlesim 거북이 이동
      ▲                                              │
      │  current_pose 갱신                            │ /turtle1/pose 발행
  [TurtleSub_Action] ◄────────────────────────────────┘
      │
      ├─ Feedback : remained_dist                 (남은 거리, 이동 중 계속)
      └─ Result   : pos_x, pos_y, pos_theta, result_dist  (도착 후 1번)

노드 2개(액션 서버 + pose 구독자)를 MultiThreadedExecutor 로 함께 실행한다.
→ 액션 콜백이 while 문으로 오래 도는 동안에도 pose 구독 콜백이 따로 실행되어야
  current_pose 가 갱신되기 때문.

실행: ros2 run my_first_package dist_turtle_action_server
요청: ros2 action send_goal --feedback /dist_turtle my_first_package_msgs/action/DistTurtle \
        "{linear_x: 0.8, angular_z: 0.4, dist: 2.0}"
"""

import time
import rclpy as rp
from rclpy.action import ActionServer                 # 액션 서버 클래스
from rclpy.executors import MultiThreadedExecutor     # 여러 콜백을 여러 스레드에서 동시에 실행
from rclpy.node import Node

from turtlesim.msg import Pose                        # 거북이 상태: x, y, theta, 속도
from geometry_msgs.msg import Twist                   # 속도 명령
from my_first_package_msgs.action import DistTurtle   # 직접 정의한 액션 (Goal / Result / Feedback)
from my_first_package.my_subscriber import TurtlesimSubscriber  # 예전에 만든 pose 구독 노드 재사용

import math
import time


class TurtleSub_Action(TurtlesimSubscriber):
    """
    TurtlesimSubscriber 를 상속한 pose 구독 노드.
    - 부모(TurtlesimSubscriber)가 이미 /turtle1/pose 구독을 만들어 두었으므로 구독 설정은 그대로 사용
    - callback 만 덮어써서(오버라이드) 좌표를 print 하는 대신
      액션 서버의 current_pose 에 최신 위치를 넣어준다.
    """

    def __init__(self, ac_server):
        super().__init__()              # 부모 __init__ 실행 → /turtle1/pose 구독 생성
        self.ac_server = ac_server      # 위치를 전달할 액션 서버 노드를 기억

    def callback(self, msg):
        # /turtle1/pose 가 올 때마다(약 60Hz) 액션 서버의 현재 위치를 갱신
        self.ac_server.current_pose = msg


class DistTurtleServer(Node):
    """Goal 을 받아 거북이를 움직이고, 이동 거리를 누적해 피드백/결과를 보내는 액션 서버."""

    def __init__(self):
        super().__init__('dist_turtle_action_server')   # 노드 이름 등록

        self.total_dist = 0             # 이번 Goal 에서 지금까지 이동한 총 거리
        self.is_first_time = True       # 거리 계산을 처음 하는지 여부 (시작점 기록용)
        self.current_pose = Pose()      # 현재 위치 (TurtleSub_Action 이 계속 갱신)
        self.previous_pose = Pose()     # 직전 계산 때의 위치

        # 거북이에게 속도 명령을 보낼 퍼블리셔
        self.publisher = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)

        # 액션 서버 생성
        #   ActionServer(노드, 액션 타입, 액션 이름, Goal 이 들어오면 실행할 함수)
        #   → 'dist_turtle' 이라는 이름으로 서버가 열리고 `ros2 action list` 에 /dist_turtle 로 보임
        self.action_server = ActionServer(self, DistTurtle, 'dist_turtle', self.excute_callback)

    def calc_diff_pose(self):
        """직전 위치 → 현재 위치 사이의 직선 거리(이번 구간 이동 거리)를 계산."""

        # 처음 호출될 때는 '직전 위치'가 없으므로 현재 위치를 시작점으로 기록
        # (이렇게 안 하면 원점 (0,0) 부터 거리가 계산되어 값이 튐)
        if self.is_first_time:
            self.previous_pose.x = self.current_pose.x
            self.previous_pose.y = self.current_pose.y
            self.is_first_time = False

        # 두 점 사이 거리 = √((x2-x1)² + (y2-y1)²)
        diff_dist = math.sqrt((self.current_pose.x - self.previous_pose.x)**2 +\
                             (self.current_pose.y - self.previous_pose.y)**2)

        # 다음 계산을 위해 지금 위치를 '직전 위치'로 저장
        self.previous_pose = self.current_pose

        return diff_dist

    def excute_callback(self, goal_handle):
        """
        클라이언트가 Goal 을 보내면 실행되는 함수 (액션의 본체).
        goal_handle : 이번 요청을 다루는 손잡이
          - goal_handle.request          → 클라이언트가 보낸 Goal 값
          - goal_handle.publish_feedback → 피드백 전송
          - goal_handle.succeed()        → 성공 상태로 표시
        return 한 값이 Result 로 클라이언트에 전달된다.
        """
        feedback_msg = DistTurtle.Feedback()    # 피드백 메시지 (remained_dist)

        # Goal 에 담긴 속도로 거북이 속도 명령 메시지를 만든다
        msg = Twist()
        msg.linear.x = goal_handle.request.linear_x     # 직진 속도
        msg.angular.z = goal_handle.request.angular_z   # 회전 속도

        # 목표 거리에 도달할 때까지 반복
        while True:
            # 1) 이번 구간 이동 거리를 누적
            self.total_dist += self.calc_diff_pose()

            # 2) 남은 거리 = 목표 거리 - 지금까지 이동한 거리 → 피드백으로 전송
            feedback_msg.remained_dist = goal_handle.request.dist - self.total_dist
            goal_handle.publish_feedback(feedback_msg)

            # 3) 거북이에게 계속 속도 명령을 보냄 (turtlesim 은 명령이 끊기면 약 1초 뒤 멈춤)
            self.publisher.publish(msg)

            # 4) 0.01초 대기 (그 사이 다른 스레드에서 pose 콜백이 current_pose 를 갱신)
            time.sleep(0.01)

            # 5) 남은 거리가 0.2 미만이면 도착으로 보고 종료
            if feedback_msg.remained_dist < 0.2:
                break

        # 액션 성공 처리
        goal_handle.succeed()

        # 최종 결과 작성: 도착 위치와 실제 이동 거리
        result = DistTurtle.Result()
        result.pos_x = self.current_pose.x
        result.pos_y = self.current_pose.y
        result.pos_theta = self.current_pose.theta
        result.result_dist = self.total_dist

        # 다음 Goal 을 위해 상태 초기화
        self.total_dist = 0
        self.is_first_time = True

        return result   # → 클라이언트에 Result 로 전달


def main(args=None):
    rp.init(args=args)                          # ROS 2 초기화

    executor = MultiThreadedExecutor()          # 멀티스레드 실행기
                                                # (단일 스레드면 액션 while 문이 도는 동안 pose 콜백이 멈춰서
                                                #  current_pose 가 갱신되지 않음 → 거리가 영원히 0)

    ac = DistTurtleServer()                     # 액션 서버 노드
    sub = TurtleSub_Action(ac_server = ac)      # pose 구독 노드 (액션 서버에 위치 전달)

    executor.add_node(sub)                      # 두 노드를 실행기에 등록
    executor.add_node(ac)

    try:
        executor.spin()                         # 두 노드의 콜백을 계속 실행 (Ctrl+C 전까지)

    finally:                                    # 종료 시(에러, Ctrl+C 포함) 정리
        executor.shutdown()
        sub.destroy_node()
        ac.destroy_node()
        rp.shutdown()


if __name__ == '__main__':
    main()
