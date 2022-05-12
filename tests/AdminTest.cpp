/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include <cachersize/Admin.hpp>
#include <cppunit/extensions/HelperMacros.h>

extern thallium::engine engine;
extern std::string cache_type;

class AdminTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( AdminTest );
    CPPUNIT_TEST( testAdminCreateCache );
    CPPUNIT_TEST_SUITE_END();

    static constexpr const char* cache_config = "{ \"path\" : \"mydb\" }";

    public:

    void setUp() {}
    void tearDown() {}

    void testAdminCreateCache() {
        cachersize::Admin admin(engine);
        std::string addr = engine.self();

        cachersize::UUID cache_id;
        // Create a valid Cache
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("admin.createCache should return a valid Cache",
                cache_id = admin.createCache(addr, 0, cache_type, cache_config));

        // Create a Cache with a wrong backend type
        cachersize::UUID bad_id;
        CPPUNIT_ASSERT_THROW_MESSAGE("admin.createCache should throw an exception (wrong backend)",
                bad_id = admin.createCache(addr, 0, "blabla", cache_config),
                cachersize::Exception);

        // Destroy the Cache
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("admin.destroyCache should not throw on valid Cache",
            admin.destroyCache(addr, 0, cache_id));

        // Destroy an invalid Cache
        CPPUNIT_ASSERT_THROW_MESSAGE("admin.destroyCache should throw on invalid Cache",
            admin.destroyCache(addr, 0, bad_id),
            cachersize::Exception);
    }
};
CPPUNIT_TEST_SUITE_REGISTRATION( AdminTest );
