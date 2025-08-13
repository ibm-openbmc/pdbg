#pragma once

#include <string>

namespace TARGETING
{
class DeviceTreeLoader
{
  public:
    DeviceTreeLoader(const std::string& path);
    ~DeviceTreeLoader();

    DeviceTreeLoader(const DeviceTreeLoader&) = delete;
    DeviceTreeLoader& operator=(const DeviceTreeLoader&) = delete;
    DeviceTreeLoader(DeviceTreeLoader&& other) = delete;
    DeviceTreeLoader& operator=(DeviceTreeLoader&& other) = delete;

    void* fdt() const noexcept
    {
        return _fdt;
    }
    size_t size() const noexcept
    {
        return _size;
    }

  private:
    int _fd{-1};
    void* _fdt{nullptr};
    size_t _size{0};
};
} // namespace TARGETING
