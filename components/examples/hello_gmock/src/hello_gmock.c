/**
 * @file hello_gmock.c
 */

#include "hello_gmock.h"

int32_t CheckGetData(void)
{
    int32_t data = 0;
    data = GetData() * 2;
    return data;
}

int32_t CheckGetByPointer(void)
{
    int32_t data = 0;
    GetByPointer(&data);
    data *= 2;
    return data;
}

int32_t CheckGetByPointerAndReturnValue(int32_t *const data)
{
    return GetByPointerAndReturnValue(data);
}

void InitDataStructure(MyDataType *const data)
{
    data->a = 42;
    data->b = 'a';
}

void CheckGetDataStructureByPointer(MyDataType *const data)
{
    GetDataStructureByPointer(data);
}

void CheckGetDataStructureArray(MyDataType *const data)
{
    GetDataStructureArray(data);
}

void CheckSetData(int32_t data)
{
    SetData(data * 2);
}

void CheckSetDataByPointer(int32_t data)
{
    int32_t localData = data * 2;
    SetDataByPointer(&localData);
}
