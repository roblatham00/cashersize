/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "cachersize/CacheHandle.hpp"
#include "cachersize/RequestResult.hpp"
#include "cachersize/Exception.hpp"

#include "AsyncRequestImpl.hpp"
#include "ClientImpl.hpp"
#include "CacheHandleImpl.hpp"

#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/pair.hpp>

namespace cachersize {

CacheHandle::CacheHandle() = default;

CacheHandle::CacheHandle(const std::shared_ptr<CacheHandleImpl>& impl)
: self(impl) {}

CacheHandle::CacheHandle(const CacheHandle&) = default;

CacheHandle::CacheHandle(CacheHandle&&) = default;

CacheHandle& CacheHandle::operator=(const CacheHandle&) = default;

CacheHandle& CacheHandle::operator=(CacheHandle&&) = default;

CacheHandle::~CacheHandle() = default;

CacheHandle::operator bool() const {
    return static_cast<bool>(self);
}

Client CacheHandle::client() const {
    return Client(self->m_client);
}

void CacheHandle::sayHello() const {
    if(not self) throw Exception("Invalid cachersize::CacheHandle object");
    auto& rpc = self->m_client->m_say_hello;
    auto& ph  = self->m_ph;
    auto& cache_id = self->m_cache_id;
    rpc.on(ph)(cache_id);
}

void CacheHandle::computeSum(
        int32_t x, int32_t y,
        int32_t* result,
        AsyncRequest* req) const
{
    if(not self) throw Exception("Invalid cachersize::CacheHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto& cache_id = self->m_cache_id;
    if(req == nullptr) { // synchronous call
        RequestResult<int32_t> response = rpc.on(ph)(cache_id, x, y);
        if(response.success()) {
            if(result) *result = response.value();
        } else {
            throw Exception(response.error());
        }
    } else { // asynchronous call
        auto async_response = rpc.on(ph).async(cache_id, x, y);
        auto async_request_impl =
            std::make_shared<AsyncRequestImpl>(std::move(async_response));
        async_request_impl->m_wait_callback =
            [result](AsyncRequestImpl& async_request_impl) {
                RequestResult<int32_t> response =
                    async_request_impl.m_async_response.wait();
                    if(response.success()) {
                        if(result) *result = response.value();
                    } else {
                        throw Exception(response.error());
                    }
            };
        *req = AsyncRequest(std::move(async_request_impl));
    }
}

}
