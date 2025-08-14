#pragma once

#include <string>

namespace TARGETING
{
/**
 * @class DeviceTreeLoader
 * @brief RAII-based loader for Flattened Device Tree (FDT) blobs.
 *
 * This class loads a device tree file from disk into memory and
 * provides access to its contents and size.
 * 
 * The loaded FDT pointer is valid for the lifetime of the
 * DeviceTreeLoader object.
 *
 * Copy and move operations are disabled to avoid multiple owners
 * of the same memory-mapped FDT blob.
 */
class DeviceTreeLoader
{
  public:
    /**
     * @brief Constructs a DeviceTreeLoader and loads the FDT from the given file path.
     *
     * Opens the file at \p path, memory maps it into the process address space,
     * and makes it accessible via fdt().
     *
     * @param[in] path Path to the device tree blob (DTB) file to load.
     * @throws std::runtime_error if the file cannot be opened or mapped.
     */
    DeviceTreeLoader(const std::string& path);

    /**
     * @brief Destructor.
     *
     * Unmaps the device tree file from memory and closes the file descriptor.
     */
    ~DeviceTreeLoader();

    /// @cond disable_copy_move
    DeviceTreeLoader(const DeviceTreeLoader&) = delete;
    DeviceTreeLoader& operator=(const DeviceTreeLoader&) = delete;
    DeviceTreeLoader(DeviceTreeLoader&& other) = delete;
    DeviceTreeLoader& operator=(DeviceTreeLoader&& other) = delete;
    /// @endcond

    /**
     * @brief Get the pointer to the loaded FDT data.
     *
     * @return Pointer to the start of the loaded device tree blob in memory.
     *         This pointer is only valid as long as the DeviceTreeLoader
     *         object exists.
     */
    void* fdt() const noexcept
    {
        return _fdt;
    }

    /**
     * @brief Get the size of the loaded FDT in bytes.
     *
     * @return Size in bytes of the loaded device tree blob.
     */
    size_t size() const noexcept
    {
        return _size;
    }

  private:
    int _fd{-1};        ///< File descriptor for the opened device tree file.
    void* _fdt{nullptr};///< Pointer to the memory-mapped FDT data.
    size_t _size{0};    ///< Size of the FDT in bytes.
};
} // namespace TARGETING

