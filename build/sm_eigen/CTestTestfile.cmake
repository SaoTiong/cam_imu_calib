# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/Schweizer-Messer/sm_eigen
# Build directory: /calibration_ws/build/sm_eigen
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_sm_eigen_gtest_sm_eigen-test "/calibration_ws/build/sm_eigen/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/sm_eigen/test_results/sm_eigen/gtest-sm_eigen-test.xml" "--return-code" "/calibration_ws/devel/.private/sm_eigen/lib/sm_eigen/sm_eigen-test --gtest_output=xml:/calibration_ws/build/sm_eigen/test_results/sm_eigen/gtest-sm_eigen-test.xml")
set_tests_properties(_ctest_sm_eigen_gtest_sm_eigen-test PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/Schweizer-Messer/sm_eigen/CMakeLists.txt;51;catkin_add_gtest;/calibration_ws/src/kalibr/Schweizer-Messer/sm_eigen/CMakeLists.txt;0;")
subdirs("gtest")
