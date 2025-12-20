# CMake generated Testfile for 
# Source directory: /calibration_ws/src/kalibr/aslam_optimizer/sparse_block_matrix
# Build directory: /calibration_ws/build/sparse_block_matrix
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(_ctest_sparse_block_matrix_gtest_sparse_block_matrix_tests "/calibration_ws/build/sparse_block_matrix/catkin_generated/env_cached.sh" "/usr/bin/python3" "/opt/ros/noetic/share/catkin/cmake/test/run_tests.py" "/calibration_ws/build/sparse_block_matrix/test_results/sparse_block_matrix/gtest-sparse_block_matrix_tests.xml" "--return-code" "/calibration_ws/devel/.private/sparse_block_matrix/lib/sparse_block_matrix/sparse_block_matrix_tests --gtest_output=xml:/calibration_ws/build/sparse_block_matrix/test_results/sparse_block_matrix/gtest-sparse_block_matrix_tests.xml")
set_tests_properties(_ctest_sparse_block_matrix_gtest_sparse_block_matrix_tests PROPERTIES  _BACKTRACE_TRIPLES "/opt/ros/noetic/share/catkin/cmake/test/tests.cmake;160;add_test;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;98;catkin_run_tests_target;/opt/ros/noetic/share/catkin/cmake/test/gtest.cmake;37;_catkin_add_google_test;/calibration_ws/src/kalibr/aslam_optimizer/sparse_block_matrix/CMakeLists.txt;35;catkin_add_gtest;/calibration_ws/src/kalibr/aslam_optimizer/sparse_block_matrix/CMakeLists.txt;0;")
subdirs("gtest")
