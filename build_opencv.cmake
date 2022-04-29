cmake_minimum_required(VERSION 3.13)

set(build_type Release)
set(source_dir ${CMAKE_CURRENT_LIST_DIR}/opencv)
set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build_opencv)
set(build_options
-DBUILD_opencv_dnn:BOOL=0
-DBUILD_opencv_gapi:BOOL=0
-DBUILD_opencv_apps:BOOL=0
-DBUILD_opencv_python_bindings_generator:BOOL=0
-DBUILD_opencv_videoio:BOOL=0
-DBUILD_opencv_python3:BOOL=0
-DBUILD_opencv_objdetect:BOOL=0
-DBUILD_opencv_flann:BOOL=0
-DBUILD_opencv_java_bindings_generator:BOOL=0
-DBUILD_TESTS:BOOL=0
-DBUILD_JAVA:BOOL=0
-DBUILD_opencv_ml:BOOL=0
-DBUILD_opencv_js_bindings_generator:BOOL=0
-DBUILD_opencv_features2d:BOOL=0
-DBUILD_opencv_ts:BOOL=0
-DBUILD_opencv_highgui:BOOL=0
-DBUILD_opencv_python_tests:BOOL=0
-DBUILD_opencv_objc_bindings_generator:BOOL=0
-DBUILD_PERF_TESTS:BOOL=0
-DBUILD_PROTOBUF:BOOL=0
-DBUILD_SHARED_LIBS:BOOL=0
-DBUILD_opencv_video:BOOL=0
-DBUILD_opencv_photo:BOOL=0
-DBUILD_opencv_calib3d:BOOL=0
-DBUILD_opencv_stitching:BOOL=0
-DWITH_ADE:BOOL=0
-DWITH_PROTOBUF:BOOL=0
-DWITH_QUIRC:BOOL=0
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${source_dir} -B ${build_dir} ${build_options} -DCMAKE_BUILD_TYPE=${build_type}
    )
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${build_dir} -DCMAKE_BUILD_TYPE=${build_type} --config ${build_type}
    )
execute_process(
    COMMAND ${CMAKE_COMMAND} --install ${build_dir} -DCMAKE_BUILD_TYPE=${build_type} --config ${build_type}
    )