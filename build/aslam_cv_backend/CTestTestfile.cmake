# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/aslam_cv/aslam_cv_backend
# Build directory: /calibration_ws/build/aslam_cv_backend
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_aslam_cv_backend_gtest_aslam_cv_backend_test "/calibration_ws/build/aslam_cv_backend/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/aslam_cv_backend/test_results/aslam_cv_backend/gtest-aslam_cv_backend_test.xml" "--return-code" "/calibration_ws/devel/.private/aslam_cv_backend/lib/aslam_cv_backend/aslam_cv_backend_test --gtest_output=xml:/calibration_ws/build/aslam_cv_backend/test_results/aslam_cv_backend/gtest-aslam_cv_backend_test.xml")
set_tests_properties(_ctest_aslam_cv_backend_gtest_aslam_cv_backend_test PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/aslam_cv/aslam_cv_backend/CMakeLists.txt;40;catkin_add_gtest;/calibration_ws/src/kalibr/aslam_cv/aslam_cv_backend/CMakeLists.txt;0;")
subdirs("gtest")
