cc_library(
   name = 'timer',
   srcs = [
       'thread_timer.cpp',
       'timer_manager.cpp',
   ],
   deps = [
   ], 
)

cc_test(
    name = 'timer_manager_test',
    srcs = [
        'timer_manager_test.cpp',
    ],
    deps = [
        ':timer',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ],
)
