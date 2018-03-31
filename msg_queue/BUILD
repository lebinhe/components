cc_library(
    name = 'msg_queue',
    srcs = [
        'msg.cpp',
        'msg_queue.cpp',
    ],
    deps = [
    ]
)

cc_test(
    name = 'msg_queue_test',
    srcs = [
        'msg_queue_test.cpp',
    ],
    deps = [
        ':msg_queue',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ],
)
