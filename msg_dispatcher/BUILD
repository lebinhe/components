cc_library(
    name = "dispatcher",
    srcs = [
        'dispatcher.cpp',
    ],
    deps = [
    ]
)

cc_test(
    name = "dispatcher_test",
    srcs = [
        'dispatcher_test.cpp',
    ],
    deps = [
        ':dispatcher',
        '//components/msg_queue:msg_queue',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)
