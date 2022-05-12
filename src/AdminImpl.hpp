/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __CACHERSIZE_ADMIN_IMPL_H
#define __CACHERSIZE_ADMIN_IMPL_H

#include <thallium.hpp>

namespace cachersize {

namespace tl = thallium;

class AdminImpl {

    public:

    tl::engine           m_engine;
    tl::remote_procedure m_create_cache;
    tl::remote_procedure m_open_cache;
    tl::remote_procedure m_close_cache;
    tl::remote_procedure m_destroy_cache;

    AdminImpl(const tl::engine& engine)
    : m_engine(engine)
    , m_create_cache(m_engine.define("cachersize_create_cache"))
    , m_open_cache(m_engine.define("cachersize_open_cache"))
    , m_close_cache(m_engine.define("cachersize_close_cache"))
    , m_destroy_cache(m_engine.define("cachersize_destroy_cache"))
    {}

    AdminImpl(margo_instance_id mid)
    : AdminImpl(tl::engine(mid)) {
    }

    ~AdminImpl() {}
};

}

#endif
