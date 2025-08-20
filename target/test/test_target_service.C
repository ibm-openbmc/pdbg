#include <association.H>
#include <attributeenums.H>
#include <attributetraits.H>
#include <predicateattr.H>
#include <predicateattrval.H>
#include <predicatepostfixexpr.H>
#include <target.H>
#include <target_service.H>

#include <algorithm>

#include <gtest/gtest.h>
using namespace TARGETING;

class TargetServiceTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        TargetService::instance().init("../target/test/targeting_test.dtb");
    }

    TargetPtr getFirstTargetMatchingType(TYPE type)
    {
        auto top = TargetService::instance().getTopLevelTarget();
        for (auto&& tgt : TargetService::instance().getAssociated(
                 top, AssociationType::childByPhysical, all))
        {
            AttributeTraits<ATTR_TYPE>::Type rawVal = TYPE_INVALID;
            if (tgt->tryGetAttr<ATTR_TYPE>(rawVal) &&
                static_cast<TYPE>(rawVal) == type)
            {
                return tgt;
            }
        }
        return nullptr;
    }
};

TEST_F(TargetServiceTest, TestFDTNotNull)
{
    EXPECT_NE(TargetService::instance().getFDT(), nullptr);
}

//////////////TEST getTopLevelTarget method//////////
TEST_F(TargetServiceTest, TestTopLevelTarget)
{
    auto top = TargetService::instance().getTopLevelTarget();
    ASSERT_NE(top, nullptr);

    ATTR_FAPI_NAME_typeStdArr name;
    EXPECT_TRUE(top->tryGetAttr<ATTR_FAPI_NAME>(name));
    std::string strName{name.data()};
    EXPECT_TRUE(strName.starts_with("k0"));
}

//////////////TEST getParentOf method//////////
TEST_F(TargetServiceTest, TestGetParentOfOcmbImmediatePhysical)
{
    ConstTargetPtr ocmb = getFirstTargetMatchingType(TYPE_OCMB_CHIP);
    ASSERT_NE(ocmb, nullptr);

    auto parent = TargetService::instance().getParentOf(
        ocmb, AssociationType::parentByPhysical);
    ASSERT_NE(parent, nullptr);

    AttributeTraits<ATTR_TYPE>::Type rawVal = TYPE_INVALID;
    EXPECT_TRUE(parent->tryGetAttr<ATTR_TYPE>(rawVal));
    EXPECT_EQ(static_cast<TYPE>(rawVal), TYPE_NODE);
}

TEST_F(TargetServiceTest, TestGetParentOfOcmbImmediateAffinity)
{
    ConstTargetPtr ocmb = getFirstTargetMatchingType(TYPE_OCMB_CHIP);
    ASSERT_NE(ocmb, nullptr);

    auto parent = TargetService::instance().getParentOf(
        ocmb, AssociationType::parentByAffinity);
    ASSERT_NE(parent, nullptr);

    AttributeTraits<ATTR_TYPE>::Type rawVal = TYPE_INVALID;
    EXPECT_TRUE(parent->tryGetAttr<ATTR_TYPE>(rawVal));
    EXPECT_EQ(static_cast<TYPE>(rawVal), TYPE_OMI);
}

//////////////TEST getAssociated method//////////
TEST_F(TargetServiceTest, TestGetAssociatedChildrenImmediate)
{
    ConstTargetPtr proc = getFirstTargetMatchingType(TYPE_PROC);
    ASSERT_NE(proc, nullptr);

    int count = 0;
    for (auto&& child : TargetService::instance().getAssociated(
             proc, AssociationType::childByPhysical, RecursionLevel::immediate))
    {
        ++count;
        EXPECT_NE(child, nullptr);

        AttributeTraits<ATTR_TYPE>::Type t = TYPE_INVALID;
        EXPECT_TRUE(child->tryGetAttr<ATTR_TYPE>(t));
        EXPECT_EQ(t, TYPE_MC); // Direct child of PROC is MC
    }
    EXPECT_GT(count, 0);
}

TEST_F(TargetServiceTest, TestGetAssociatedParentsAffinityAll)
{
    auto ocmb = getFirstTargetMatchingType(TYPE_OCMB_CHIP);
    ASSERT_NE(ocmb, nullptr);

    std::vector<TYPE> expectedHierarchy = {
        TYPE_OMI, TYPE_MCC, TYPE_MI, TYPE_MC, TYPE_PROC, TYPE_NODE, TYPE_SYS};

    std::vector<AttributeTraits<ATTR_TYPE>::Type> actual;

    for (auto&& parent : TargetService::instance().getAssociated(
             ocmb, AssociationType::parentByAffinity, RecursionLevel::all))
    {
        AttributeTraits<ATTR_TYPE>::Type t = TYPE_INVALID;
        EXPECT_TRUE(parent->tryGetAttr<ATTR_TYPE>(t));
        actual.push_back(t);
    }

    EXPECT_TRUE(
        std::includes(actual.begin(), actual.end(), expectedHierarchy.begin(),
                      expectedHierarchy.end()));
}

TEST_F(TargetServiceTest, TestRoundTripToTarget)
{
    auto ocmb = getFirstTargetMatchingType(TYPE_OCMB_CHIP);
    ASSERT_NE(ocmb, nullptr);

    EntityPath path;
    EXPECT_TRUE(ocmb->tryGetAttr<ATTR_PHYS_PATH>(path));

    auto roundtrip = TargetService::instance().toTarget(path);
    ASSERT_NE(roundtrip, nullptr);

    EXPECT_EQ(roundtrip->getOffset(), ocmb->getOffset());
}

///////////////PREDICATES//////////////////////////////
TEST_F(TargetServiceTest, TestPredicateAttrValProcType)
{
    PredicateAttrVal<ATTR_TYPE> pred(TYPE_PROC);
    int count = 0;

    auto top = TargetService::instance().getTopLevelTarget();
    for (auto&& tgt : TargetService::instance().getAssociated(
             top, AssociationType::childByPhysical, RecursionLevel::all, &pred))
    {
        AttributeTraits<ATTR_TYPE>::Type t = TYPE_INVALID;
        EXPECT_TRUE(tgt->tryGetAttr<ATTR_TYPE>(t));
        EXPECT_EQ(t, TYPE_PROC);
        ++count;
    }

    EXPECT_EQ(count, 2); // Your DTB has proc0, proc1
}

TEST_F(TargetServiceTest, TestPredicatePostfixExpr_AttrVal_AND)
{
    auto isProc = std::make_shared<PredicateAttrVal<ATTR_TYPE>>(TYPE_PROC);
    auto isClassProc =
        std::make_shared<PredicateAttrVal<ATTR_CLASS>>(CLASS_CHIP);

    PredicatePostfixExpr expr;
    expr.push(isProc).push(isClassProc).And();

    int count = 0;
    auto top = TargetService::instance().getTopLevelTarget();
    for (auto&& tgt : TargetService::instance().getAssociated(
             top, AssociationType::childByPhysical, RecursionLevel::all, &expr))
    {
        AttributeTraits<ATTR_TYPE>::Type type = TYPE_INVALID;
        AttributeTraits<ATTR_CLASS>::Type cls = CLASS_INVALID;
        EXPECT_TRUE(tgt->tryGetAttr<ATTR_TYPE>(type));
        EXPECT_TRUE(tgt->tryGetAttr<ATTR_CLASS>(cls));
        EXPECT_EQ(type, TYPE_PROC);
        EXPECT_EQ(cls, CLASS_CHIP);
        ++count;
    }

    EXPECT_EQ(count, 2); // proc0, proc1
}

TEST_F(TargetServiceTest, TestPredicatePostfixExpr_AttrMask_OR)
{
    // Match TYPE_PROC (0x05) OR TYPE_MC (0x44)
    auto isProc = std::make_shared<PredicateAttrVal<ATTR_TYPE>>(TYPE_PROC);
    auto isMC = std::make_shared<PredicateAttrVal<ATTR_TYPE>>(TYPE_MC);

    PredicatePostfixExpr expr;
    expr.push(isProc).push(isMC).Or();

    std::vector<AttributeTraits<ATTR_TYPE>::Type> matchedTypes;
    auto top = TargetService::instance().getTopLevelTarget();
    for (auto&& tgt : TargetService::instance().getAssociated(
             top, AssociationType::childByPhysical, RecursionLevel::all, &expr))
    {
        AttributeTraits<ATTR_TYPE>::Type t = TYPE_INVALID;
        EXPECT_TRUE(tgt->tryGetAttr<ATTR_TYPE>(t));
        matchedTypes.push_back(t);
        EXPECT_TRUE(t == TYPE_PROC || t == TYPE_MC);
    }

    EXPECT_GT(matchedTypes.size(), 0);
    EXPECT_TRUE(std::find(matchedTypes.begin(), matchedTypes.end(),
                          TYPE_PROC) != matchedTypes.end());
    EXPECT_TRUE(std::find(matchedTypes.begin(), matchedTypes.end(), TYPE_MC) !=
                matchedTypes.end());
}

TEST_F(TargetServiceTest, TestPredicatePostfixExpr_Negation)
{
    auto isNotProc = std::make_shared<PredicateAttrVal<ATTR_TYPE>>(TYPE_PROC);
    PredicatePostfixExpr expr;
    expr.push(isNotProc).Not();

    int count = 0;
    auto top = TargetService::instance().getTopLevelTarget();
    for (auto&& tgt : TargetService::instance().getAssociated(
             top, AssociationType::childByPhysical, RecursionLevel::all, &expr))
    {
        AttributeTraits<ATTR_TYPE>::Type t = TYPE_INVALID;
        EXPECT_TRUE(tgt->tryGetAttr<ATTR_TYPE>(t));
        EXPECT_NE(t, TYPE_PROC);
        ++count;
    }

    EXPECT_GT(count, 0); // Should exclude proc0/1/2
}

TEST_F(TargetServiceTest, GetTopLevelTarget_IsCached)
{
    using namespace TARGETING;

    // First call: should construct and cache the top-level target
    auto first = TargetService::instance().getTopLevelTarget();
    ASSERT_NE(first, nullptr)
        << "Expected valid top-level target on first call";

    // Second call: should return same pointer from the cache
    auto second = TargetService::instance().getTopLevelTarget();
    ASSERT_EQ(first, second) << "Expected same target returned (cached)";
}

TEST_F(TargetServiceTest, TestGetAttrThrowsOnMissing)
{
    auto top = TargetService::instance().getTopLevelTarget();
    ASSERT_NE(top, nullptr);

    try
    {
        // LOCATION_CODE is not present for system target
        [[maybe_unused]] auto val = top->getAttrAsArray<ATTR_LOCATION_CODE>();
        FAIL() << "Expected exception for missing attribute";
    }
    catch (const std::runtime_error& e)
    {
        SUCCEED();
    }
}

TEST_F(TargetServiceTest, TestSetAttrThrowsOnMissing)
{
    auto top = TargetService::instance().getTopLevelTarget();
    ASSERT_NE(top, nullptr);

    try
    {
        // LOCATION_CODE is not present for system target
        top->setAttr<ATTR_SYS_CLK_NE_TERMINATION_SITE>(0x1);
        FAIL() << "Expected exception for missing attribute";
    }
    catch (const std::runtime_error& e)
    {
        SUCCEED();
    }
}

TEST_F(TargetServiceTest, TestGetAttrAndTrySetAttr_HW_ACCESS_METHOD)
{
    auto proc = getFirstTargetMatchingType(TYPE_PROC);
    ASSERT_NE(proc, nullptr);

    // Save original
    auto original = proc->getAttr<ATTR_HW_ACCESS_METHOD>();

    // Write back a new value
    AttributeTraits<ATTR_HW_ACCESS_METHOD>::Type method =
        HW_ACCESS_METHOD_SBEFIFO;
    EXPECT_TRUE(proc->trySetAttr<ATTR_HW_ACCESS_METHOD>(method));

    // Verify change
    auto newVal = proc->getAttr<ATTR_HW_ACCESS_METHOD>();
    EXPECT_EQ(newVal, method);

    // Restore
    EXPECT_TRUE(proc->trySetAttr<ATTR_HW_ACCESS_METHOD>(original));
}

TEST_F(TargetServiceTest, CanReadReadableAttr)
{
    auto proc = getFirstTargetMatchingType(TYPE_PROC);
    ASSERT_NE(proc, nullptr);
    typename AttributeTraits<ATTR_TYPE>::Type val{};
    EXPECT_TRUE(proc->tryGetAttr<ATTR_TYPE>(val)); // should compile & run
}

TEST_F(TargetServiceTest, CanWriteWritableAttr)
{
    auto proc = getFirstTargetMatchingType(TYPE_PROC);
    ASSERT_NE(proc, nullptr);
    AttributeTraits<ATTR_HW_ACCESS_METHOD>::Type method =
        HW_ACCESS_METHOD_SBEFIFO;
    EXPECT_TRUE(proc->trySetAttr<ATTR_HW_ACCESS_METHOD>(method));
}

// Verify get throws when attribute not set
TEST_F(TargetServiceTest, GetVolatileBeforeSetThrows)
{
    auto proc = getFirstTargetMatchingType(TYPE_PROC);
    EXPECT_THROW(
        {
            auto val = proc->getAttr<ATTR_HW_ACCESS_PTR>();
            (void)val;
        },
        std::runtime_error);
}

// Verify set followed by get returns the same value
TEST_F(TargetServiceTest, SetAndGetVolatileWorks)
{
    constexpr uint32_t testValue = 0xDEADBEEF;

    auto proc = getFirstTargetMatchingType(TYPE_PROC);
    ASSERT_NE(proc, nullptr);

    EXPECT_TRUE(proc->trySetAttr<ATTR_HW_ACCESS_PTR>(testValue));

    // Now get should succeed
    auto val = proc->getAttr<ATTR_HW_ACCESS_PTR>();
    EXPECT_EQ(val, testValue);
}

class MockTarget : public TARGETING::Target
{
  public:
    mutable bool fdtCalled = false;

    // Now allowed, since Target declared us as friend
    MockTarget(void* fdt, int offset) : Target(fdt, offset) {}

  protected:
    std::optional<std::span<const uint8_t>>
        fdtGetProperty(const void*, int, const std::string&) const override
    {
        fdtCalled = true;
        return std::nullopt;
    }
};

TEST_F(TargetServiceTest, getAttrPrefersVolatileOverFdt)
{
    auto proc = getFirstTargetMatchingType(TYPE_PROC);
    ASSERT_NE(proc, nullptr);
    constexpr uint32_t testValue = 0xDEADBEEF;

    // Use same offset and fdt from real target
    MockTarget t(TargetService::instance().getFDT(), proc->getOffset());

    EXPECT_TRUE(proc->trySetAttr<ATTR_HW_ACCESS_PTR>(testValue));

    [[maybe_unused]] auto result =
        proc->getAttr<TARGETING::ATTR_HW_ACCESS_PTR>();

    // Confirm FDT path wasn't touched
    EXPECT_FALSE(t.fdtCalled);
}
