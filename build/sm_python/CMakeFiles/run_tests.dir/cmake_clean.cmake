file(REMOVE_RECURSE
  "/calibration_ws/devel/.private/sm_python/lib/python3/dist-packages/sm//calibration_ws/devel/.private/sm_python/lib/libsm_python.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/run_tests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
