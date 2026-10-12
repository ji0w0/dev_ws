# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/TCPClient_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/TCPClient_autogen.dir/ParseCache.txt"
  "TCPClient_autogen"
  )
endif()
