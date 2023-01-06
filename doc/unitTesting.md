# Unit Testing

## Writing Test Code

Test code (TC) shall be written in the same language as the productive code (PC) and put in a folder 'test' in the components folder. This is a template for a test suite written in C using the available test framework [Unity](http://www.throwtheswitch.org/unity):

```C
#include <unity.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void test_implement_me(void)
{
    UNITY_TEST_ASSERT_EQUAL(0, 1, "This fails!");
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_implement_me);

    return UNITY_END();
}
```

If you put that testsuite in your component's test folder as test_component.c you can add it to the build in parts.cmake:

```cmake
add_source(src/component.c)

add_test_source(test/test_component.c)
```

## Mocking

In order to successfully implement your tests, it is very likely that you must mock (https://en.wikipedia.org/wiki/Mock_object#Motivation) some of your used interfaces. We provide a way of semi-automatic mocking. In case of `IsolMn.c` we managed to add mocks by:
- adding `create_mocks(test/mock/Rte_IsolMn.h)` to `src/App/IsolMn/parts.cmake`
- creating the file `test/mock/Rte_IsolMn.h` inside IsolMn component
  - this file is a copy of the real `Rte_IsolMn.h`
  - but with the Autosar functions "manually" preprocessed. For details compare both files with BeyondCompare or similar
-  add all missing global variables to `test_component.c` (previously created) so that it links.

# Building and Executing Unit Tests

The execution of a unit test is just a target in the build system, but it is part of another _kit_. So (1) select the _variant_, (2) change _kit_ to 'test' (cmake reconfigure might be necessary after this step: ctrl+shift+p --> 'cmake delete cache and reconfigure'), (3) choose the test to run (e.g. 'src_app_component_unittests' for one specific or 'unittests' for all) and (4) click on build:

![vscode-build](img/vscode-test.png)

![vscode-build](img/run-unittest.gif)
