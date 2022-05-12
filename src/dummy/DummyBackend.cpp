/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "DummyBackend.hpp"
#include <iostream>

CACHERSIZE_REGISTER_BACKEND(dummy, DummyCache);

void DummyCache::sayHello() {
    std::cout << "Hello World" << std::endl;
}

cachersize::RequestResult<int32_t> DummyCache::computeSum(int32_t x, int32_t y) {
    cachersize::RequestResult<int32_t> result;
    result.value() = x + y;
    return result;
}

cachersize::RequestResult<bool> DummyCache::destroy() {
    cachersize::RequestResult<bool> result;
    result.value() = true;
    // or result.success() = true
    return result;
}

std::unique_ptr<cachersize::Backend> DummyCache::create(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<cachersize::Backend>(new DummyCache(config));
}

std::unique_ptr<cachersize::Backend> DummyCache::open(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<cachersize::Backend>(new DummyCache(config));
}
