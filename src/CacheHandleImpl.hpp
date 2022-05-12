/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __CACHERSIZE_CACHE_HANDLE_IMPL_H
#define __CACHERSIZE_CACHE_HANDLE_IMPL_H

#include <cachersize/UUID.hpp>

namespace cachersize {

class CacheHandleImpl {

    public:

    UUID                        m_cache_id;
    std::shared_ptr<ClientImpl> m_client;
    tl::provider_handle         m_ph;

    CacheHandleImpl() = default;
    
    CacheHandleImpl(const std::shared_ptr<ClientImpl>& client, 
                       tl::provider_handle&& ph,
                       const UUID& cache_id)
    : m_cache_id(cache_id)
    , m_client(client)
    , m_ph(std::move(ph)) {}
};

}

#endif
