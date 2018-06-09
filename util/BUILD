cc_library(
    name = 'coding',
    srcs = 'coding.cpp',
    deps = [
    ],
)

cc_library(
    name = 'hash',
    srcs = 'hash.cpp',
    deps = [
    ],
)

cc_library(
    name = 'arena',
    srcs = 'arena.cpp',
    deps = [
    ],
)

cc_library(
    name = 'status',
    srcs = 'status.cpp',
    deps = [
    ],
)

cc_library(
    name = 'env',
    srcs = [
        'env.cpp',
        'env_posix.cpp',
    ],
    deps = [
        '//components/util:status',
    ]
)

cc_library(
    name = 'filter',
    srcs = [
        'bloom.cpp',
        'filter_policy.cpp',
    ],
    deps = [
        '//components/util:hash',
    ]
)

cc_library(
    name = 'crc32c',
    srcs = 'crc32c.cpp',
    deps = [
        ':coding',
    ]
)

cc_library(
    name = 'logging',
    srcs = 'logging.cpp',
    deps = [
    ]
)

cc_test(
    name = 'coding_test',
    srcs = 'coding_test.cpp',
    deps = [
        ':coding',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ],
)

cc_test(
    name = 'hash_test',
    srcs = 'hash_test.cpp',
    deps = [
        ':hash',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'arena_test',
    srcs = 'arena_test.cpp',
    deps = [
        ':arena',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'status_test',
    srcs = 'status_test.cpp',
    deps = [
        ':status',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'env_test',
    srcs = 'env_test.cpp',
    deps = [
        ':env',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'env_posix_test',
    srcs = 'env_posix_test.cpp',
    deps = [
        ':env',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'bloom_test',
    srcs = 'bloom_test.cpp',
    deps = [
        ':filter',
        ':coding',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'crc32c_test',
    srcs = 'crc32c_test.cpp',
    deps = [
        ':crc32c',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)

cc_test(
    name = 'logging_test',
    srcs = 'logging_test.cpp',
    deps = [
        ':logging',
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
    ]
)
