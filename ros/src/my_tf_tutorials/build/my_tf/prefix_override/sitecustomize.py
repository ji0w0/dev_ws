import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/jiu/Git-projects/dev_ws/ros/src/my_tf_tutorials/install/my_tf'
