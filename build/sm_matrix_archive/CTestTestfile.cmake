# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/Schweizer-Messer/sm_matrix_archive
# Build directory: /calibration_ws/build/sm_matrix_archive
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_sm_matrix_archive_gtest_sm_matrix_archive-test "/calibration_ws/build/sm_matrix_archive/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/sm_matrix_archive/test_results/sm_matrix_archive/gtest-sm_matrix_archive-test.xml" "--return-code" "/calibration_ws/devel/.private/sm_matrix_archive/lib/sm_matrix_archive/sm_matrix_archive-test --gtest_output=xml:/calibration_ws/build/sm_matrix_archive/test_results/sm_matrix_archive/gtest-sm_matrix_archive-test.xml")
set_tests_properties(_ctest_sm_matrix_archive_gtest_sm_matrix_archive-test PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/Schweizer-Messer/sm_matrix_archive/CMakeLists.txt;54;catkin_add_gtest;/calibration_ws/src/kalibr/Schweizer-Messer/sm_matrix_archive/CMakeLists.txt;0;")
subdirs("gtest")
