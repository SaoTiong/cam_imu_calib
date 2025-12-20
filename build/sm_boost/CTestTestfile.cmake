# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/Schweizer-Messer/sm_boost
# Build directory: /calibration_ws/build/sm_boost
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_sm_boost_gtest_sm_boost-test "/calibration_ws/build/sm_boost/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/sm_boost/test_results/sm_boost/gtest-sm_boost-test.xml" "--return-code" "/calibration_ws/devel/.private/sm_boost/lib/sm_boost/sm_boost-test --gtest_output=xml:/calibration_ws/build/sm_boost/test_results/sm_boost/gtest-sm_boost-test.xml")
set_tests_properties(_ctest_sm_boost_gtest_sm_boost-test PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/Schweizer-Messer/sm_boost/CMakeLists.txt;52;catkin_add_gtest;/calibration_ws/src/kalibr/Schweizer-Messer/sm_boost/CMakeLists.txt;0;")
subdirs("gtest")
