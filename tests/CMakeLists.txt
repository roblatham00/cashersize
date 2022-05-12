include_directories(../include)

add_library(cachersize-test Main.cpp)
target_link_libraries(cachersize-test PkgConfig::cppunit cachersize-server cachersize-admin cachersize-client)

add_executable(AdminTest AdminTest.cpp)
target_link_libraries(AdminTest cachersize-test)

add_executable(ClientTest AdminTest.cpp)
target_link_libraries(ClientTest cachersize-test)

add_executable(CacheTest CacheTest.cpp)
target_link_libraries(CacheTest cachersize-test)

add_test(NAME AdminTest COMMAND ./AdminTest AdminTest.xml)
add_test(NAME ClientTest COMMAND ./ClientTest ClientTest.xml)
add_test(NAME CacheTest COMMAND ./CacheTest CacheTest.xml)
