cmake_minimum_required(VERSION 3.13)

set(build_type Release)
set(source_dir ${CMAKE_CURRENT_LIST_DIR})
set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)

execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${source_dir} -B ${build_dir} -DCMAKE_BUILD_TYPE=${build_type}
    )
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${build_dir} -DCMAKE_BUILD_TYPE=${build_type} --config ${build_type}
    )
# execute_process(
#     COMMAND ${CMAKE_COMMAND} --install ${build_dir} -DCMAKE_BUILD_TYPE=${build_type} --config ${build_type}
#     )