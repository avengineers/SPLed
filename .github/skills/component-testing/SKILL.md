---
name: component-testing
description: Guide for testing C components in the SPLED environment using GTest/GMock. Use when user needs to design test cases, write tests, debug test failures, add test traceability, or understand testing strategy for a component. Covers test design, traceability to requirements, mock setup, and debugging patterns.
---

# Component Testing Guide

This skill guides developers through the complete testing workflow for SPLED components: from understanding requirements and designing test cases, to writing traced tests with proper mock setup, to debugging failures.

## Trigger Conditions

- User asks to test a component or write/add tests
- User mentions test failures, debugging tests, or test coverage
- User asks about test strategy, test design, or what to test
- User asks about traceability, linking tests to requirements, or test documentation
- User mentions GTest, GMock, mocks, or test infrastructure

## Technical Context

- **Build System**: `build.ps1` wrapper, CMake with SPL macros, `test` build kit for unit tests
- **Auto-Mocker**: Build system generates `mockup_components_<component_name>.h` and `.cc` for each component based on missing symbols when partial linking the component source files
- **Mock Instantiation**: Use `CREATE_MOCK(mymock);` macro (not manual instantiation) - this is project-specific
- **CMake Macros**: `spl_add_test_source(test/test_<name>.cc)` registers test files, `spl_add_required_interface(components/<dep>)` declares dependencies

## Testing Workflow

### 1. Understand Requirements & Design Test Strategy

Before writing code, understand what needs to be tested:

1. **Find design documentation**: Look for `components/<name>/doc/index.md` - this contains Software Detailed Design (SWDD) requirements
2. **Identify requirements**: Extract requirement IDs (format: `SWDD_<COMP>-###`) that need test coverage
3. **Analyze component behavior**: Read `components/<name>/src/<name>.c` and `.h` to understand:
   - State machines or behavioral logic
   - Edge cases and boundary conditions
   - Error handling paths
   - KConfig-conditional behavior
4. **Design test cases**: For each requirement, identify:
   - **Positive tests**: Normal operation paths
   - **Negative tests**: Error conditions, invalid inputs
   - **Boundary tests**: Min/max values, state transitions
   - **Sequence tests**: Ordered interactions (debouncing, state changes)

**Ask the user**: "Which requirements should I focus on testing?" or "Should I analyze the design doc to identify untested requirements?"

### 2. Analyze Component Dependencies

Inspect the component to understand its external interfaces:

1. Read `components/<name>/CMakeLists.txt`
2. Extract all `spl_add_required_interface(components/<dep>)` calls - these are the dependencies that will be mocked
3. Identify conditional dependencies (wrapped in `if(FEATURE STREQUAL "True")` blocks) - these need `#ifdef CONFIG_FEATURE` guards in tests
4. Read `components/<name>/src/<name>.h` to understand the function signatures of the component under test
5. (Optional) Read interface headers from `components/<dep>/src/<dep>.h` to understand function signatures for `EXPECT_CALL` setups

### 3. Write Traced Test Cases

Create or update `components/<name>/test/test_<name>.cc` with full traceability.

#### Pattern A: Simple Tests (No Fixture Needed)

Use `TEST()` for stateless tests that don't need setup/teardown:

```cpp
#include <gtest/gtest.h>

extern "C"
{
#include "autoconf.h"  // For KConfig feature flags
#include "<component_name>.h"
}

#include "mockup_components_<component_name>.h"  // Provides GMock functionality

/*!
 * @rst
 *
 * .. test:: <component_name>.test_basic_behavior
 *    :id: TS_<COMP>-001
 *    :tests: SWDD_<COMP>-100, SWDD_<COMP>-201
 *
 * Brief description of what this test validates.
 *
 * @endrst
 */
TEST(<component_name>, test_basic_behavior)
{
    CREATE_MOCK(mymock);

    // Set expectations
    EXPECT_CALL(mymock, InterfaceFunction(arg)).WillRepeatedly(Return(value));

    // Call function under test
    <componentName>();
}
```

#### Pattern B: Tests with Fixtures

Use `TEST_F()` when you need shared setup/teardown or state:

```cpp
class <ComponentName>Test : public Test
{
protected:
    void SetUp() override
    {
        // Initialize component
        <componentName>Init();
    }

    void TearDown() override
    {
        // Cleanup if needed
    }
};

/*!
 * @rst
 *
 * .. test:: <ComponentName>Test.DescriptiveTestName
 *    :id: TS_<COMP>-002
 *    :tests: SWDD_<COMP>-300
 *
 * Brief description.
 *
 * @endrst
 */
TEST_F(<ComponentName>Test, DescriptiveTestName)
{
    CREATE_MOCK(mymock);
    EXPECT_CALL(mymock, InterfaceFunction(_)).WillOnce(Return(0));
    <componentName>();
}
```

#### Pattern C: Parameterized Tests (DRY Approach)

Use `TEST_P()` to avoid duplicating test logic for multiple input scenarios:

```cpp
// Define test parameters structure
struct TestParam
{
    const char *description;
    input_type_t input;
    expected_type_t expected;
};

// Overload operator<< with description for pinpointing failing test instances
// This makes GTest output show "TestName/TestFixture.TestName/MinValue" instead of generic index
std::ostream &operator<<(std::ostream &os, const TestParam &param)
{
    os << param.description;
    return os;
}

// Define fixture for parameterized test
class <ComponentName>ParamTest : public TestWithParam<TestParam>
{
};

/*!
 * @rst
 *
 * .. test:: <ComponentName>Tests/<ComponentName>ParamTest.ValidatesMultipleScenarios/*
 *    :id: TS_<COMP>-003
 *    :tests: SWDD_<COMP>-400
 *
 * Parameterized test covering multiple input scenarios.
 *
 * @endrst
 */
TEST_P(<ComponentName>ParamTest, ValidatesMultipleScenarios)
{
    TestParam param = GetParam();

    result_t actual = functionUnderTest(param.input);
    EXPECT_EQ(actual, param.expected) << "Test case: " << param.description;
}

// Instantiate test suite with parameters
INSTANTIATE_TEST_SUITE_P(
    <ComponentName>Tests,
    <ComponentName>ParamTest,
    Values(
        TestParam{"MinValue", 0, expectedMin},
        TestParam{"MidValue", 50, expectedMid},
        TestParam{"MaxValue", 100, expectedMax}
    ));
```

#### Pattern D: Custom Matchers for Complex Types

For comparing structs or complex types:

```cpp
// Helper function
bool areStructsEqual(const MyStruct *a, const MyStruct *b)
{
    return a->field1 == b->field1 && a->field2 == b->field2;
}

// Custom matcher
MATCHER_P(MyStructEq, expected, "")
{
    return areStructsEqual(&arg, &expected);
}

// Overload operator<< for better error messages
std::ostream &operator<<(std::ostream &os, const MyStruct &s)
{
    os << "MyStruct(" << s.field1 << ", " << s.field2 << ")";
    return os;
}

// Usage in test
TEST(<component_name>, test_struct_comparison)
{
    CREATE_MOCK(mymock);
    MyStruct expected = {.field1 = 10, .field2 = 20};

    EXPECT_CALL(mymock, SetValue(MyStructEq(expected))).Times(1);
    <componentName>();
}
```

#### KConfig-Conditional Tests

```cpp
#ifdef CONFIG_FEATURE_NAME
/*!
 * @rst
 *
 * .. test:: <component_name>.test_feature_specific
 *    :id: TS_<COMP>-004
 *    :tests: SWDD_<COMP>-500
 *
 * Tests behavior when FEATURE_NAME is enabled.
 *
 * @endrst
 */
TEST(<component_name>, test_feature_specific)
{
    CREATE_MOCK(mymock);
    // Test code that only runs when FEATURE_NAME is enabled
}
#endif
```

**Traceability Documentation Pattern**:

- **RST block**: Must appear immediately before each `TEST()`, `TEST_F()`, or `TEST_P()`
- **Test name matching**: The `.. test::` name MUST exactly match the GTest test name for traceability to work in test reports:
  - For `TEST(SuiteName, TestName)`: Use `.. test:: SuiteName.TestName`
  - For `TEST_F(FixtureName, TestName)`: Use `.. test:: FixtureName.TestName`
  - For `TEST_P(FixtureName, TestName)`: Use `.. test:: TestSuiteName/FixtureName.TestName/*` (wildcard pattern to trace all parameter instances)
- **Test ID**: Format `TS_<COMPONENT_ABBREV>-###` (sequential numbers, pad to 3 digits)
- **Requirement links**: `:tests:` field lists all SWDD requirements validated by this test (comma-separated)
- **Description**: Brief explanation of what the test verifies
- **Finding requirements**: Read `components/<name>/doc/index.md` to get SWDD IDs

**Key Testing Patterns**:

- **Test naming**: Use descriptive names with `test_` prefix (e.g., `test_light_stays_off`, `test_press_event_sent_only_after_debounce`)
- **Fixture choice**: Use `TEST()` for stateless tests, `TEST_F()` when you need setup/teardown, `TEST_P()` for parameterized tests
- **DRY principle**: Prefer `TEST_P()` over multiple similar tests - define parameters once, test logic once
- **Mock instantiation**: Use `CREATE_MOCK(mymock);` inside each test (not as class member)
- **Headers**: Include `gtest/gtest.h` and `autoconf.h` for KConfig flags, wrap C headers in `extern "C" {}`
- **Mock header**: `#include "mockup_components_<component_name>.h"` (singular, matches component name) - this provides GMock functionality, no need to include `gmock/gmock.h`
- **Expectations**: Use `WillOnce()` for single calls, `WillRepeatedly()` for loops or multiple calls
- **Ordered calls**: Use `InSequence seq;` before expectations when call order matters
- **Loop patterns**: Use `for` loops to simulate time passing or repeated state (e.g., `for (int i = 0; i < 49; i++)`)
- **Custom matchers**: Define `MATCHER_P()` for complex type comparisons (e.g., structs)
- **Test output**: Overload `operator<<` for custom types to get readable failure messages
- **Helper functions**: Extract comparison logic into helper functions (e.g., `areRGBColorsEqual()`)
- **KConfig guards**: Wrap conditional feature tests in `#ifdef CONFIG_<FEATURE>`

### 4. Update CMakeLists.txt

Ensure the test file is registered for the `test` build kit:

1. Check `components/<name>/CMakeLists.txt`
2. Add `spl_add_test_source(test/test_<name>.cc)` if not present (usually after `spl_add_source()`)
3. Verify `spl_add_required_interface()` declarations match the actual dependencies in the C code

### 5. Build and Debug Tests

Run tests using the `test` build kit:

```powershell
.\build.ps1 -build -buildKit test -buildType Debug -variants <VariantName> -target components_<component_name>_unittest
```

**Common Test Failures & Fixes**:

- **Uninteresting mock function call**: Missing `EXPECT_CALL` for a function. Add expectation or verify the component shouldn't call it.
- **Unexpected call**: Component calls function more/fewer times than expected. Check `Times()` modifier or logic.
- **Wrong call order**: Use `InSequence seq;` to enforce ordering, or check if order actually matters.
- **Linker errors for mock functions**: Mock header not included, or interface not in `spl_add_required_interface()`.
- **CONFIG_X not defined**: Missing `#include "autoconf.h"` in test file.

## Critical Testing Principles

**Traceability is Mandatory**:

- Every test must have an RST documentation block with test ID and requirement links
- Test IDs follow format: `TS_<COMPONENT_ABBREV>-###` (e.g., `TS_PB-001` for power button)
- Link to all validated requirements in `:tests:` field
- Check design doc (`components/<name>/doc/index.md`) for requirement IDs

**Test Design Philosophy**:

- **Test behavior, not implementation**: Focus on inputs/outputs and state changes
- **One concept per test**: Each test should validate one specific behavior or requirement
- **Use descriptive names**: Test name should explain the scenario without reading the code
- **Cover edge cases**: State transitions, boundary values, error paths, timing (debounce patterns)

**Mock Strategy**:

- `CREATE_MOCK(mymock)` provides strict mocking - all calls must be expected
- Strict mocking catches unexpected interactions early (better than permissive mocks)
- If component calls an interface conditionally, guard expectations with `#ifdef` or conditional logic

## Rules for Agents

- **ALWAYS** add RST traceability block before each `TEST()`, `TEST_F()`, or `TEST_P()` with test ID and requirement links
- **ALWAYS** use `CREATE_MOCK(mymock);` - never instantiate `StrictMock<>` manually
- **ALWAYS** include `autoconf.h` when tests reference KConfig features
- **ALWAYS** match test guards (`#ifdef CONFIG_X`) to CMakeLists.txt conditionals (`if(X STREQUAL "True")`)
- **ALWAYS** read the design doc (`components/<name>/doc/index.md`) to find requirements to test
- **PREFER** `TEST_P()` parameterized tests when testing same logic with multiple inputs (DRY principle)
- **PREFER** `TEST()` for simple stateless tests; use `TEST_F()` only when setup/teardown is needed
- **SUGGEST** custom matchers (`MATCHER_P`) for complex type comparisons instead of manual field-by-field checks
- **SUGGEST** overloading `operator<<` for custom types to improve test failure messages
- **DO NOT** use standard CMake commands (`add_executable`, `target_link_libraries`) - only SPL macros
- **DO NOT** forget `extern "C" {}` wrapper around C headers
- **DO NOT** write tests without traceability documentation
- **DO NOT** duplicate test logic - use `TEST_P()` instead
- Reference existing tests: `components/light_controller/test/test_light_controller.cc`, `components/power_button/test/test_power_button.cc`
