#pragma once

namespace TARGETING
{

enum AssociationType
{
    parentByPhysical,
    childByPhysical,
    parentByAffinity,
    childByAffinity,
};

enum RecursionLevel
{
    immediate = 0x01,
    all = 0x02,
};

} // namespace TARGETING
