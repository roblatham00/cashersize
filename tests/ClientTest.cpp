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

class ClientTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ClientTest );
    CPPUNIT_TEST( testOpenCache );
    CPPUNIT_TEST_SUITE_END();

    static constexpr const char* cache_config = "{ \"path\" : \"mydb\" }";
    UUID cache_id;

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

    void testOpenCache() {
        cachersize::Client client(engine);
        std::string addr = engine.self();
        
        Cache my_cache = client.open(addr, 0, cache_id);
        CPPUNIT_ASSERT_MESSAGE(
                "Cache should be valid",
                static_cast<bool>(my_cache));

        auto bad_id = UUID::generate();
        CPPUNIT_ASSERT_THROW_MESSAGE(
                "client.open should fail on non-existing cache",
                client.open(addr, 0, bad_id);
    }
};
CPPUNIT_TEST_SUITE_REGISTRATION( ClientTest );
