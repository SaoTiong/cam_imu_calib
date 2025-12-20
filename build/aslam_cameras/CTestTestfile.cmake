# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/aslam_cv/aslam_cameras
# Build directory: /calibration_ws/build/aslam_cameras
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_aslam_cameras_gtest_aslam_cameras_tests "/calibration_ws/build/aslam_cameras/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/aslam_cameras/test_results/aslam_cameras/gtest-aslam_cameras_tests.xml" "--working-dir" "/calibration_ws/src/kalibr/aslam_cv/aslam_cameras/test" "--return-code" "/calibration_ws/devel/.private/aslam_cameras/lib/aslam_cameras/aslam_cameras_tests --gtest_output=xml:/calibration_ws/build/aslam_cameras/test_results/aslam_cameras/gtest-aslam_cameras_tests.xml")
set_tests_properties(_ctest_aslam_cameras_gtest_aslam_cameras_tests PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/aslam_cv/aslam_cameras/CMakeLists.txt;59;catkin_add_gtest;/calibration_ws/src/kalibr/aslam_cv/aslam_cameras/CMakeLists.txt;0;")
subdirs("gtest")
