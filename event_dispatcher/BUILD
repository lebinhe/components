cc_library(
    name = "dispatcher",
    srcs = [
        'dispatcher.cpp',
        'subscriber.cpp',
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
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)
