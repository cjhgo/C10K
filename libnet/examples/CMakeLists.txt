

add_libnet_executable(test_log test00_log.cpp)


add_libnet_executable(test_loop test01_ioloop.cpp)


add_libnet_executable(test_loop2 test02_ioloop.cpp)


add_libnet_executable(test_channel test03_channel.cpp)


add_libnet_executable(test_runtimer test04_runtimer.cpp)

add_libnet_executable(test_runinloop test05_runinloop.cpp)

add_libnet_executable(test_runinother test05_00callinother.cpp)


add_libnet_executable(test_loopinthread test06_eventloopthread.cpp)


add_libnet_executable(test_acceptor test07_acceptor.cpp)

add_libnet_executable(test_onmessage test08_handlemessage.cpp)


add_libnet_executable(test_echo  test09_echoserver.cpp)

add_libnet_executable(test_send  test10_send.cpp)

add_libnet_executable(test_wccb  test11_wccb.cpp)