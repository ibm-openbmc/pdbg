#include "entitypath.H"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <sstream>

namespace TARGETING
{

EntityPath::EntityPath(PathType type)
{
    setType(type);
    setSize(0);
}

EntityPath& EntityPath::removeLast()
{
    assert(getSize() > 0);
    _elements[getSize() - 1] = {};
    setSize(getSize() - 1);
    return *this;
}

EntityPath EntityPath::copyRemoveLast() const
{
    EntityPath copy = *this;
    copy.removeLast();
    return copy;
}

EntityPath& EntityPath::addLast(TYPE type, uint8_t instance)
{
    assert(getSize() < MAX_PATH_ELEMENTS);
    PathElement pe(type, instance);
    _elements[getSize()] = pe;
    setSize(getSize() + 1);
    return *this;
}

EntityPath EntityPath::copyAddLast(TYPE type, uint8_t instance) const
{
    EntityPath copy = *this;
    copy.addLast(type, instance);
    return copy;
}

bool EntityPath::operator==(const EntityPath& rhs) const
{
    return type() == rhs.type() && getSize() == rhs.getSize() &&
           std::equal(std::begin(_elements), std::begin(_elements) + getSize(),
                      std::begin(rhs._elements));
}

bool EntityPath::operator<(const EntityPath& rhs) const
{
    if (type() != rhs.type())
        return type() < rhs.type();

    int cmp =
        std::memcmp(_elements, rhs._elements,
                    std::min(getSize(), rhs.getSize()) * sizeof(PathElement));

    if (cmp != 0)
        return cmp < 0;

    return getSize() < rhs.getSize();
}

bool EntityPath::equals(const EntityPath& rhs, std::size_t count) const
{
    assert(count <= MAX_PATH_ELEMENTS);
    return type() == rhs.type() && count <= getSize() &&
           count <= rhs.getSize() &&
           std::equal(std::begin(_elements), std::begin(_elements) + count,
                      std::begin(rhs._elements));
}

const EntityPath::PathElement& EntityPath::operator[](std::size_t index) const
{
    assert(index < getSize());
    return _elements[index];
}

void EntityPath::replace(std::size_t index, const PathElement& element)
{
    assert(index < getSize());
    _elements[index] = element;
}

EntityPath::PathElement EntityPath::pathElementOfType(TYPE type) const
{
    for (uint32_t i = 0; i < getSize(); ++i)
    {
        if (_elements[i].type() == type)
        {
            return _elements[i];
        }
    }
    return {};
}

std::string EntityPath::pathTypeAsString() const
{
    switch (type())
    {
        case PathType::Device:
            return "Device:";
        case PathType::Affinity:
            return "Logical:";
        case PathType::Physical:
            return "Physical:";
        case PathType::Power:
            return "Power:";
        default:
            return "Unknown entity path type";
    }
}

std::string EntityPath::pathElementTypeAsString(TYPE type) const
{
    switch (type)
    {
        case TYPE_NA:
            return "NA";
        case TYPE_SYS:
            return "Sys";
        case TYPE_NODE:
            return "Node";
        case TYPE_DIMM:
            return "DIMM";
        case TYPE_MEMBUF:
            return "Membuf";
        case TYPE_PROC:
            return "Proc";
        case TYPE_FC:
            return "FC";
        case TYPE_EX:
            return "EX";
        case TYPE_CORE:
            return "Core";
        case TYPE_L2:
            return "L2";
        case TYPE_L3:
            return "L3";
        case TYPE_L4:
            return "L4";
        case TYPE_MCS:
            return "MCS";
        case TYPE_MBA:
            return "MBA";
        case TYPE_XBUS:
            return "XBUS";
        case TYPE_ABUS:
            return "ABUS";
        case TYPE_PCI:
            return "PCI";
        case TYPE_DPSS:
            return "DPSS";
        case TYPE_APSS:
            return "APSS";
        case TYPE_OCC:
            return "OCC";
        case TYPE_PSI:
            return "PSI";
        case TYPE_FSP:
            return "FSP";
        case TYPE_PNOR:
            return "PNOR";
        case TYPE_OSC:
            return "OSC";
        case TYPE_MFREFCLK:
            return "MFREFClock";
        case TYPE_TODCLK:
            return "TodClock";
        case TYPE_CONTROL_NODE:
            return "Control Node";
        case TYPE_NX:
            return "NX";
        case TYPE_PORE:
            return "PORE";
        case TYPE_OSCREFCLK:
            return "OSCREFClock";
        case TYPE_OSCPCICLK:
            return "OSCPCIClock";
        case TYPE_REFCLKENDPT:
            return "REFClockEndPoint";
        case TYPE_PCICLKENDPT:
            return "PCIClockEndPoint";
        case TYPE_PCIESWITCH:
             return "PCIESWITCH";
        case TYPE_CAPP:
            return "CAPP";
        case TYPE_FSI:
            return "FSI";
        case TYPE_EQ:
            return "EQ";
        case TYPE_MCA:
            return "MCA";
        case TYPE_MCBIST:
            return "MCBIST";
        case TYPE_MC:
            return "MC";
        case TYPE_MI:
            return "MI";
        case TYPE_DMI:
            return "DMI";
        case TYPE_OBUS:
            return "OBUS";
        case TYPE_OBUS_BRICK:
            return "OBUS_BRICK";
        case TYPE_NPU:
            return "NPU";
        case TYPE_SBE:
            return "SBE";
        case TYPE_PPE:
            return "PPE";
        case TYPE_PERV:
            return "PERV";
        case TYPE_PEC:
            return "PEC";
        case TYPE_PHB:
            return "PHB";
        case TYPE_SYSREFCLKENDPT:
            return "SYSREFCLKENDPT";
        case TYPE_MFREFCLKENDPT:
            return "MFREFCLKENDPT";
        case TYPE_TPM:
            return "TPM";
        case TYPE_SP:
            return "SP";
        case TYPE_UART:
            return "UART";
        case TYPE_PS:
            return "PS";
        case TYPE_FAN:
            return "FAN";
        case TYPE_VRM:
            return "VRM";
        case TYPE_USB:
            return "USB";
        case TYPE_ETH:
            return "ETH";
        case TYPE_PANEL:
            return "PANEL";
        case TYPE_BMC:
            return "BMC";
        case TYPE_FLASH:
            return "FLASH";
        case TYPE_SEEPROM:
            return "SEEPROM";
        case TYPE_TMP:
            return "TMP";
        case TYPE_GPIO_EXPANDER:
            return "GPIO_EXPANDER";
        case TYPE_POWER_SEQUENCER:
            return "POWER_SEQUENCER";
        case TYPE_RTC:
            return "RTC";
        case TYPE_FANCTLR:
            return "FANCTLR";
        case TYPE_SMPGROUP:
            return "SMPGROUP";
        case TYPE_OMI:
            return "OMI";
        case TYPE_OMIC:
            return "OMIC";
        case TYPE_MCC:
            return "MCC";
        case TYPE_OCMB_CHIP:
            return "OCMB_CHIP";
        case TYPE_MEM_PORT:
            return "MEM_PORT";
        case TYPE_I2C_MUX:
            return "I2C_MUX";
        case TYPE_PMIC:
            return "PMIC";
        case TYPE_NMMU:
            return "NMMU";
        case TYPE_PAU:
            return "PAU";
        case TYPE_IOHS:
            return "IOHS";
        case TYPE_PAUC:
            return "PAUC";
        case TYPE_LPCREFCLKENDPT:
            return "LPCREFCLKENDPT";
        case TYPE_GENERIC_I2C_DEVICE:
            return "GENERIC_I2C_DEVICE";
        case TYPE_MDS_CTLR:
            return "MDS_CTLR";
        case TYPE_DCM:
            return "DCM";
        case TYPE_TEMP_SENSOR:
            return "TEMP_SENSOR";
        case TYPE_POWER_IC:
            return "POWER_IC";
        default:
            return "Unknown path type";
    }
}

std::string EntityPath::toString() const
{
    std::ostringstream oss;
    oss << pathTypeAsString();
    for (uint32_t i = 0; i < getSize(); ++i)
    {
        oss << '/' << pathElementTypeAsString(_elements[i].type())
            << static_cast<int>(_elements[i].instance);
    }
    return oss.str();
}

// In EntityPath.C
EntityPath EntityPath::fromBinary(std::span<const uint8_t> data)
{
    EntityPath path;

    if (data.size() < 1)
        return path;

    uint8_t header = data[0];
    uint8_t pathType = (header >> 4) & LOWER_NIBBLE_MASK;
    uint8_t elementCount = header & LOWER_NIBBLE_MASK;

    std::size_t expectedSize = 1 + 2 * elementCount;
    if (data.size() < expectedSize)
        return path;

    path.setType(static_cast<PathType>(pathType));

    auto elements = data.subspan(1, 2 * elementCount);
    for (size_t i = 0; i < elementCount; ++i)
    {
        TYPE type = static_cast<TYPE>(elements[i * 2]);
        uint8_t instance = elements[i * 2 + 1];
        path.addLast(type, instance); // this increments size correctly
    }

    return path;
}
} // namespace TARGETING
