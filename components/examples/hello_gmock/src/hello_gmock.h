#ifndef HELLO_GMOCK_H
#define HELLO_GMOCK_H

#include <stdint.h>

extern int32_t GetData(void);
extern int32_t CheckGetData(void);

extern void GetByPointer(int32_t *const data);
extern int32_t CheckGetByPointer(void);

extern int32_t GetByPointerAndReturnValue(int32_t *const data);
extern int32_t CheckGetByPointerAndReturnValue(int32_t *const data);

typedef struct
{
    int32_t a;
    char b;
} MyDataType;

extern void InitDataStructure(MyDataType *const data);
extern void GetDataStructureByPointer(MyDataType *const data);
extern void CheckGetDataStructureByPointer(MyDataType *const data);

extern void GetDataStructureArray(MyDataType *const data);
extern void CheckGetDataStructureArray(MyDataType *const data);

extern void SetData(int32_t data);
extern void CheckSetData(int32_t data);

extern void SetDataByPointer(int32_t *const data);
extern void CheckSetDataByPointer(int32_t data);

#endif /* HELLO_GMOCK_H */
