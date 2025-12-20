# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/aslam_offline_calibration/kalibr
# Build directory: /calibration_ws/build/kalibr
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_kalibr_gtest_kalibr_test "/calibration_ws/build/kalibr/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/kalibr/test_results/kalibr/gtest-kalibr_test.xml" "--return-code" "/calibration_ws/devel/.private/kalibr/lib/kalibr/kalibr_test --gtest_output=xml:/calibration_ws/build/kalibr/test_results/kalibr/gtest-kalibr_test.xml")
set_tests_properties(_ctest_kalibr_gtest_kalibr_test PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/aslam_offline_calibration/kalibr/CMakeLists.txt;51;catkin_add_gtest;/calibration_ws/src/kalibr/aslam_offline_calibration/kalibr/CMakeLists.txt;0;")
subdirs("gtest")
