/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cachersize/Client.hpp>
#include <cachersize/Admin.hpp>

extern thallium::engine engine;
extern std::string cache_type;

class CacheTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CacheTest );
    CPPUNIT_TEST( testMakeCacheHandle );
    CPPUNIT_TEST( testSayHello );
    CPPUNIT_TEST( testComputeSum );
    CPPUNIT_TEST_SUITE_END();

    static constexpr const char* cache_config = "{ \"path\" : \"mydb\" }";
    cachersize::UUID cache_id;

    public:

    void setUp() {
        cachersize::Admin admin(engine);
        std::string addr = engine.self();
        cache_id = admin.createCache(addr, 0, cache_type, cache_config);
    }

    void tearDown() {
        cachersize::Admin admin(engine);
        std::string addr = engine.self();
        admin.destroyCache(addr, 0, cache_id);
    }

    void testMakeCacheHandle() {
        cachersize::Client client(engine);
        std::string addr = engine.self();

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "client.makeCacheHandle should not throw for valid id.",
                client.makeCacheHandle(addr, 0, cache_id));

        auto bad_id = cachersize::UUID::generate();
        CPPUNIT_ASSERT_THROW_MESSAGE(
                "client.makeCacheHandle should throw for invalid id.",
                client.makeCacheHandle(addr, 0, bad_id),
                cachersize::Exception);
        
        CPPUNIT_ASSERT_THROW_MESSAGE(
                "client.makeCacheHandle should throw for invalid provider.",
                client.makeCacheHandle(addr, 1, cache_id),
                std::exception);
        
        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "client.makeCacheHandle should not throw for invalid id when check=false.",
                client.makeCacheHandle(addr, 0, bad_id, false));

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "client.makeCacheHandle should not throw for invalid provider when check=false.",
                client.makeCacheHandle(addr, 1, cache_id, false));
    }

    void testSayHello() {
        cachersize::Client client(engine);
        std::string addr = engine.self();
        
        cachersize::CacheHandle my_cache = client.makeCacheHandle(addr, 0, cache_id);

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_cache.sayHello() should not throw.",
                my_cache.sayHello());
    }

    void testComputeSum() {
        cachersize::Client client(engine);
        std::string addr = engine.self();
        
        cachersize::CacheHandle my_cache = client.makeCacheHandle(addr, 0, cache_id);

        int32_t result = 0;
        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_cache.computeSum() should not throw.",
                my_cache.computeSum(42, 51, &result));

        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "42 + 51 should be 93",
                93, result);

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_cache.computeSum() should not throw when passed NULL.",
                my_cache.computeSum(42, 51, nullptr));

        cachersize::AsyncRequest request;
        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "my_cache.computeSum() should not throw when called asynchronously.",
                my_cache.computeSum(42, 51, &result, &request));

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(
                "request.wait() should not throw.",
                request.wait());
    }

};
CPPUNIT_TEST_SUITE_REGISTRATION( CacheTest );
