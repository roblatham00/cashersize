/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __CACHERSIZE_ADMIN_HPP
#define __CACHERSIZE_ADMIN_HPP

#include <nlohmann/json.hpp>
#include <thallium.hpp>
#include <string>
#include <memory>
#include <cachersize/Exception.hpp>
#include <cachersize/UUID.hpp>

namespace cachersize {

namespace tl = thallium;

class AdminImpl;

/**
 * @brief Admin interface to a CACHERSIZE service. Enables creating
 * and destroying caches, and attaching and detaching them
 * from a provider. If CACHERSIZE providers have set up a security
 * token, operations from the Admin interface will need this
 * security token.
 */
class Admin {

    public:

    using json = nlohmann::json;

    /**
     * @brief Default constructor.
     */
    Admin();

    /**
     * @brief Constructor using a margo instance id.
     *
     * @param mid Margo instance id.
     */
    Admin(margo_instance_id mid);

    /**
     * @brief Constructor.
     *
     * @param engine Thallium engine.
     */
    Admin(const tl::engine& engine);
    
    /**
     * @brief Copy constructor.
     */
    Admin(const Admin&);

    /**
     * @brief Move constructor.
     */
    Admin(Admin&&);

    /**
     * @brief Copy-assignment operator.
     */
    Admin& operator=(const Admin&);

    /**
     * @brief Move-assignment operator.
     */
    Admin& operator=(Admin&&);

    /**
     * @brief Destructor.
     */
    ~Admin();

    /**
     * @brief Check if the Admin instance is valid.
     */
    operator bool() const;
    
    /**
     * @brief Creates a cache on the target provider.
     * The config string must be a JSON object acceptable
     * by the desired backend's creation function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the cache to create.
     * @param config JSON configuration for the cache.
     */
    UUID createCache(const std::string& address,
                        uint16_t provider_id,
                        const std::string& type,
                        const std::string& config,
                        const std::string& token="") const;
    
    /**
     * @brief Creates a cache on the target provider.
     * The config string must be a JSON object acceptable
     * by the desired backend's creation function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the cache to create.
     * @param config JSON configuration for the cache.
     */
    UUID createCache(const std::string& address,
                        uint16_t provider_id,
                        const std::string& type,
                        const char* config,
                        const std::string& token="") const {
        return createCache(address, provider_id, type, std::string(config), token);
    }

    /**
     * @brief Creates a cache on the target provider.
     * The config object must be a JSON object acceptable
     * by the desired backend's creation function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the cache to create.
     * @param config JSON configuration for the cache.
     */
    UUID createCache(const std::string& address,
                        uint16_t provider_id,
                        const std::string& type,
                        const json& config,
                        const std::string& token="") const {
        return createCache(address, provider_id, type, config.dump(), token);
    }

    /**
     * @brief Opens an existing cache in the target provider.
     * The config string must be a JSON object acceptable
     * by the desired backend's open function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the cache to create.
     * @param config JSON configuration for the cache.
     */
    UUID openCache(const std::string& address,
                      uint16_t provider_id,
                      const std::string& type,
                      const std::string& config,
                      const std::string& token="") const;

    /**
     * @brief Opens an existing database to the target provider.
     * The config object must be a JSON object acceptable
     * by the desired backend's open function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the cache to create.
     * @param config JSON configuration for the database.
     */
    UUID openCache(const std::string& address,
                      uint16_t provider_id,
                      const std::string& type,
                      const json& config,
                      const std::string& token="") const {
        return openCache(address, provider_id, type, config.dump(), token);
    }

    /**
     * @brief Closes an open cache in the target provider.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param cache_id UUID of the cache to close.
     */
    void closeCache(const std::string& address,
                        uint16_t provider_id,
                        const UUID& cache_id,
                        const std::string& token="") const;

    /**
     * @brief Destroys an open cache in the target provider.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param cache_id UUID of the cache to destroy.
     */
    void destroyCache(const std::string& address,
                         uint16_t provider_id,
                         const UUID& cache_id,
                         const std::string& token="") const;

    /**
     * @brief Shuts down the target server. The Thallium engine
     * used by the server must have remote shutdown enabled.
     *
     * @param address Address of the server to shut down.
     */
    void shutdownServer(const std::string& address) const;

    private:

    std::shared_ptr<AdminImpl> self;
};

}

#endif
