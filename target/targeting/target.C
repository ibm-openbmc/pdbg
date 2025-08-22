#include <target.H>

#include <cstring>
#include <stdexcept>

namespace TARGETING
{
Target::Target(void* fdt, int offset) : _fdt(fdt), _offset(offset) {}

int Target::getOffset() const noexcept
{
    return _offset;
}

std::optional<std::span<const uint8_t>> Target::fdtGetProperty(
    const void* fdt, int offset, const std::string& name) const
{
    int len = 0;
    const uint8_t* prop = static_cast<const uint8_t*>(
        fdt_getprop(fdt, offset, name.c_str(), &len));
    if (!prop || len <= 0)
    {
        return std::nullopt;
    }
    return std::span<const uint8_t>(prop, len);
}

void Target::fdtSetProperty(void* fdt, int offset, const std::string& name,
                            const std::span<const uint8_t>& data) const
{
    int rc = fdt_setprop(fdt, offset, name.c_str(), data.data(), data.size());
    if (rc < 0)
    {
        throw std::runtime_error("fdtSetProperty failed for " + name);
    }
}

void Target::fdtSetStringProperty(void* fdt, int offset,
                                  const std::string& name,
                                  const std::string& value) const
{
    int rc = fdt_setprop(fdt, offset, name.c_str(), value.c_str(),
                         value.length() + 1);
    if (rc < 0)
    {
        throw std::runtime_error("fdtSetStringProperty failed for " + name);
    }
}

void Target::fdtSetRawProperty(void* fdt, int offset, const std::string& name,
                               const void* data, size_t size) const
{
    int rc = fdt_setprop(fdt, offset, name.c_str(), data, size);
    if (rc < 0)
    {
        throw std::runtime_error("fdtSetRawProperty failed for " + name);
    }
}
} // namespace TARGETING
