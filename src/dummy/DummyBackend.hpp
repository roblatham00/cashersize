/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __DUMMY_BACKEND_HPP
#define __DUMMY_BACKEND_HPP

#include <cachersize/Backend.hpp>

using json = nlohmann::json;

/**
 * Dummy implementation of an cachersize Backend.
 */
class DummyCache : public cachersize::Backend {
   
    json m_config;

    public:

    /**
     * @brief Constructor.
     */
    DummyCache(const json& config)
    : m_config(config) {}

    /**
     * @brief Move-constructor.
     */
    DummyCache(DummyCache&&) = default;

    /**
     * @brief Copy-constructor.
     */
    DummyCache(const DummyCache&) = default;

    /**
     * @brief Move-assignment operator.
     */
    DummyCache& operator=(DummyCache&&) = default;

    /**
     * @brief Copy-assignment operator.
     */
    DummyCache& operator=(const DummyCache&) = default;

    /**
     * @brief Destructor.
     */
    virtual ~DummyCache() = default;

    /**
     * @brief Prints Hello World.
     */
    void sayHello() override;

    /**
     * @brief Compute the sum of two integers.
     *
     * @param x first integer
     * @param y second integer
     *
     * @return a RequestResult containing the result.
     */
    cachersize::RequestResult<int32_t> computeSum(int32_t x, int32_t y) override;

    /**
     * @brief Destroys the underlying cache.
     *
     * @return a RequestResult<bool> instance indicating
     * whether the database was successfully destroyed.
     */
    cachersize::RequestResult<bool> destroy() override;

    /**
     * @brief Static factory function used by the CacheFactory to
     * create a DummyCache.
     *
     * @param engine Thallium engine
     * @param config JSON configuration for the cache
     *
     * @return a unique_ptr to a cache
     */
    static std::unique_ptr<cachersize::Backend> create(const thallium::engine& engine, const json& config);

    /**
     * @brief Static factory function used by the CacheFactory to
     * open a DummyCache.
     *
     * @param engine Thallium engine
     * @param config JSON configuration for the cache
     *
     * @return a unique_ptr to a cache
     */
    static std::unique_ptr<cachersize::Backend> open(const thallium::engine& engine, const json& config);
};

#endif
