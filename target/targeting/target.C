#include "target.H"

namespace TARGETING::internal
{

template <>
bool tryGetAttrHelper<std::string>(const void* fdt, int offset,
                                   const std::string& propName,
                                   std::string& outVal)
{
    int len = 0;
    const char* prop = static_cast<const char*>(
        fdt_getprop(fdt, offset, propName.c_str(), &len));

    if (!prop || len <= 0)
        return false;

    outVal.assign(prop, len);
    return true;
}

template <>
bool tryGetAttrHelper<EntityPath>(const void* fdt, int offset,
                                  const std::string& propName,
                                  EntityPath& outVal)
{
    int len = 0;
    const uint8_t* prop = reinterpret_cast<const uint8_t*>(
        fdt_getprop(fdt, offset, propName.c_str(), &len));

    if (!prop || len <= 0)
        return false;

    std::span<const uint8_t> data(prop, static_cast<size_t>(len));
    outVal = EntityPath::fromBinary(data);
    return outVal.getSize() > 0;
}

} // namespace TARGETING::internal
