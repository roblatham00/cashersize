/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __CACHERSIZE_CACHE_HANDLE_HPP
#define __CACHERSIZE_CACHE_HANDLE_HPP

#include <thallium.hpp>
#include <memory>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <cachersize/Client.hpp>
#include <cachersize/Exception.hpp>
#include <cachersize/AsyncRequest.hpp>

namespace cachersize {

namespace tl = thallium;

class Client;
class CacheHandleImpl;

/**
 * @brief A CacheHandle object is a handle for a remote cache
 * on a server. It enables invoking the cache's functionalities.
 */
class CacheHandle {

    friend class Client;

    public:

    /**
     * @brief Constructor. The resulting CacheHandle handle will be invalid.
     */
    CacheHandle();

    /**
     * @brief Copy-constructor.
     */
    CacheHandle(const CacheHandle&);

    /**
     * @brief Move-constructor.
     */
    CacheHandle(CacheHandle&&);

    /**
     * @brief Copy-assignment operator.
     */
    CacheHandle& operator=(const CacheHandle&);

    /**
     * @brief Move-assignment operator.
     */
    CacheHandle& operator=(CacheHandle&&);

    /**
     * @brief Destructor.
     */
    ~CacheHandle();

    /**
     * @brief Returns the client this database has been opened with.
     */
    Client client() const;


    /**
     * @brief Checks if the CacheHandle instance is valid.
     */
    operator bool() const;

    /**
     * @brief Sends an RPC to the cache to make it print a hello message.
     */
    void sayHello() const;

    /**
     * @brief Requests the target cache to compute the sum of two numbers.
     * If result is null, it will be ignored. If req is not null, this call
     * will be non-blocking and the caller is responsible for waiting on
     * the request.
     *
     * @param[in] x first integer
     * @param[in] y second integer
     * @param[out] result result
     * @param[out] req request for a non-blocking operation
     */
    void computeSum(int32_t x, int32_t y,
                    int32_t* result = nullptr,
                    AsyncRequest* req = nullptr) const;

    private:

    /**
     * @brief Constructor is private. Use a Client object
     * to create a CacheHandle instance.
     *
     * @param impl Pointer to implementation.
     */
    CacheHandle(const std::shared_ptr<CacheHandleImpl>& impl);

    std::shared_ptr<CacheHandleImpl> self;
};

}

#endif
