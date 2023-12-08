#ifndef __component_a_h__
#define __component_a_h__

extern void ReadSomeData(int* var);
extern int CheckReadSomeData();
extern void CheckWriteSomeData(int var);

extern unsigned char ReadSomeDataAndReturn(int* var);
extern int CheckReadSomeDataAndReturn();

extern unsigned char ReadMultipleDataAndReturn(int* var1, int* var2);
extern int CheckReadMultipleDataAndReturn();

typedef struct {
    int a;
    unsigned char b;
} MyDataType;

extern void ReadDataStructure(MyDataType* var);
extern void CheckReadDataStructure(MyDataType* output);

#define MY_DATA_ARRAY_SIZE 3

extern void ReadDataStructureArray(MyDataType* var);
extern void CheckReadDataStructureArray(MyDataType* output);

extern void Read_MyInput(int* var);
extern void Write_MyInput(int* var);
extern void CalculateSquare(void);

#endif // __component_a_h__
