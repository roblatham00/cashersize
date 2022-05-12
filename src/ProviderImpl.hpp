/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __CACHERSIZE_PROVIDER_IMPL_H
#define __CACHERSIZE_PROVIDER_IMPL_H

#include "cachersize/Backend.hpp"
#include "cachersize/UUID.hpp"

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <tuple>

#define FIND_CACHE(__var__) \
        std::shared_ptr<Backend> __var__;\
        do {\
            std::lock_guard<tl::mutex> lock(m_backends_mtx);\
            auto it = m_backends.find(cache_id);\
            if(it == m_backends.end()) {\
                result.success() = false;\
                result.error() = "Cache with UUID "s + cache_id.to_string() + " not found";\
                req.respond(result);\
                spdlog::error("[provider:{}] Cache {} not found", id(), cache_id.to_string());\
                return;\
            }\
            __var__ = it->second;\
        }while(0)

namespace cachersize {

using namespace std::string_literals;
namespace tl = thallium;

class ProviderImpl : public tl::provider<ProviderImpl> {

    auto id() const { return get_provider_id(); } // for convenience

    using json = nlohmann::json;

    public:

    std::string          m_token;
    tl::pool             m_pool;
    // Admin RPC
    tl::remote_procedure m_create_cache;
    tl::remote_procedure m_open_cache;
    tl::remote_procedure m_close_cache;
    tl::remote_procedure m_destroy_cache;
    // Client RPC
    tl::remote_procedure m_check_cache;
    tl::remote_procedure m_say_hello;
    tl::remote_procedure m_compute_sum;
    // Backends
    std::unordered_map<UUID, std::shared_ptr<Backend>> m_backends;
    tl::mutex m_backends_mtx;

    ProviderImpl(const tl::engine& engine, uint16_t provider_id, const tl::pool& pool)
    : tl::provider<ProviderImpl>(engine, provider_id)
    , m_pool(pool)
    , m_create_cache(define("cachersize_create_cache", &ProviderImpl::createCache, pool))
    , m_open_cache(define("cachersize_open_cache", &ProviderImpl::openCache, pool))
    , m_close_cache(define("cachersize_close_cache", &ProviderImpl::closeCache, pool))
    , m_destroy_cache(define("cachersize_destroy_cache", &ProviderImpl::destroyCache, pool))
    , m_check_cache(define("cachersize_check_cache", &ProviderImpl::checkCache, pool))
    , m_say_hello(define("cachersize_say_hello", &ProviderImpl::sayHello, pool))
    , m_compute_sum(define("cachersize_compute_sum",  &ProviderImpl::computeSum, pool))
    {
        spdlog::trace("[provider:{0}] Registered provider with id {0}", id());
    }

    ~ProviderImpl() {
        spdlog::trace("[provider:{}] Deregistering provider", id());
        m_create_cache.deregister();
        m_open_cache.deregister();
        m_close_cache.deregister();
        m_destroy_cache.deregister();
        m_check_cache.deregister();
        m_say_hello.deregister();
        m_compute_sum.deregister();
        spdlog::trace("[provider:{}]    => done!", id());
    }

    void createCache(const tl::request& req,
                        const std::string& token,
                        const std::string& cache_type,
                        const std::string& cache_config) {

        spdlog::trace("[provider:{}] Received createCache request", id());
        spdlog::trace("[provider:{}]    => type = {}", id(), cache_type);
        spdlog::trace("[provider:{}]    => config = {}", id(), cache_config);

        auto cache_id = UUID::generate();
        RequestResult<UUID> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        json json_config;
        try {
            json_config = json::parse(cache_config);
        } catch(json::parse_error& e) {
            result.error() = e.what();
            result.success() = false;
            spdlog::error("[provider:{}] Could not parse cache configuration for cache {}",
                    id(), cache_id.to_string());
            req.respond(result);
            return;
        }

        std::unique_ptr<Backend> backend;
        try {
            backend = CacheFactory::createCache(cache_type, get_engine(), json_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            spdlog::error("[provider:{}] Error when creating cache {} of type {}:",
                    id(), cache_id.to_string(), cache_type);
            spdlog::error("[provider:{}]    => {}", id(), result.error());
            req.respond(result);
            return;
        }

        if(not backend) {
            result.success() = false;
            result.error() = "Unknown cache type "s + cache_type;
            spdlog::error("[provider:{}] Unknown cache type {} for cache {}",
                    id(), cache_type, cache_id.to_string());
            req.respond(result);
            return;
        } else {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);
            m_backends[cache_id] = std::move(backend);
            result.value() = cache_id;
        }
        
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully created cache {} of type {}",
                id(), cache_id.to_string(), cache_type);
    }

    void openCache(const tl::request& req,
                      const std::string& token,
                      const std::string& cache_type,
                      const std::string& cache_config) {

        spdlog::trace("[provider:{}] Received openCache request", id());
        spdlog::trace("[provider:{}]    => type = {}", id(), cache_type);
        spdlog::trace("[provider:{}]    => config = {}", id(), cache_config);

        auto cache_id = UUID::generate();
        RequestResult<UUID> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        json json_config;
        try {
            json_config = json::parse(cache_config);
        } catch(json::parse_error& e) {
            result.error() = e.what();
            result.success() = false;
            spdlog::error("[provider:{}] Could not parse cache configuration for cache {}",
                    id(), cache_id.to_string());
            req.respond(result);
            return;
        }

        std::unique_ptr<Backend> backend;
        try {
            backend = CacheFactory::openCache(cache_type, get_engine(), json_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            spdlog::error("[provider:{}] Error when opening cache {} of type {}:",
                    id(), cache_id.to_string(), cache_type);
            spdlog::error("[provider:{}]    => {}", id(), result.error());
            req.respond(result);
            return;
        }

        if(not backend) {
            result.success() = false;
            result.error() = "Unknown cache type "s + cache_type;
            spdlog::error("[provider:{}] Unknown cache type {} for cache {}",
                    id(), cache_type, cache_id.to_string());
            req.respond(result);
            return;
        } else {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);
            m_backends[cache_id] = std::move(backend);
            result.value() = cache_id;
        }
        
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully created cache {} of type {}",
                id(), cache_id.to_string(), cache_type);
    }

    void closeCache(const tl::request& req,
                        const std::string& token,
                        const UUID& cache_id) {
        spdlog::trace("[provider:{}] Received closeCache request for cache {}",
                id(), cache_id.to_string());

        RequestResult<bool> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);

            if(m_backends.count(cache_id) == 0) {
                result.success() = false;
                result.error() = "Cache "s + cache_id.to_string() + " not found";
                req.respond(result);
                spdlog::error("[provider:{}] Cache {} not found", id(), cache_id.to_string());
                return;
            }

            m_backends.erase(cache_id);
        }
        req.respond(result);
        spdlog::trace("[provider:{}] Cache {} successfully closed", id(), cache_id.to_string());
    }
    
    void destroyCache(const tl::request& req,
                         const std::string& token,
                         const UUID& cache_id) {
        RequestResult<bool> result;
        spdlog::trace("[provider:{}] Received destroyCache request for cache {}", id(), cache_id.to_string());

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);

            if(m_backends.count(cache_id) == 0) {
                result.success() = false;
                result.error() = "Cache "s + cache_id.to_string() + " not found";
                req.respond(result);
                spdlog::error("[provider:{}] Cache {} not found", id(), cache_id.to_string());
                return;
            }

            result = m_backends[cache_id]->destroy();
            m_backends.erase(cache_id);
        }

        req.respond(result);
        spdlog::trace("[provider:{}] Cache {} successfully destroyed", id(), cache_id.to_string());
    }

    void checkCache(const tl::request& req,
                       const UUID& cache_id) {
        spdlog::trace("[provider:{}] Received checkCache request for cache {}", id(), cache_id.to_string());
        RequestResult<bool> result;
        FIND_CACHE(cache);
        result.success() = true;
        req.respond(result);
        spdlog::trace("[provider:{}] Code successfully executed on cache {}", id(), cache_id.to_string());
    }

    void sayHello(const tl::request& req,
                  const UUID& cache_id) {
        spdlog::trace("[provider:{}] Received sayHello request for cache {}", id(), cache_id.to_string());
        RequestResult<bool> result;
        FIND_CACHE(cache);
        cache->sayHello();
        spdlog::trace("[provider:{}] Successfully executed sayHello on cache {}", id(), cache_id.to_string());
    }

    void computeSum(const tl::request& req,
                    const UUID& cache_id,
                    int32_t x, int32_t y) {
        spdlog::trace("[provider:{}] Received sayHello request for cache {}", id(), cache_id.to_string());
        RequestResult<int32_t> result;
        FIND_CACHE(cache);
        result = cache->computeSum(x, y);
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully executed computeSum on cache {}", id(), cache_id.to_string());
    }

};

}

#endif
