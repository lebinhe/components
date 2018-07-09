cc_library(
    name = 'tcp_socket',
    srcs = [
        'protocol.cpp',
        'protocol_simple.cpp',
        'socket.cpp',
    ],
    deps = [
    ],
    extra_cppflags = [
        '-std=c++11'
    ]
)

cc_binary(
    name = 'client',
    srcs = 'client.cpp',
    deps = [
        ':tcp_socket',
    ],
    extra_cppflags = [
        '-std=c++11',
    ]
)

cc_binary(
    name = 'server',
    srcs = 'server.cpp',
    deps = [
        ':tcp_socket',
    ],
    extra_cppflags = [
        '-std=c++11',
    ]
)
