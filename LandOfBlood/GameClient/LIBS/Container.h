#pragma once
#include <string>
#include <vector>
#include "Utils.h"

enum ContainerType {
    BOOL_ = 0,
    INT_ = 1,
    STRING_ = 2,
    VECT2_ = 3,
    CONTAINER_ = 4
};
std::string TypeToString(ContainerType type);

struct ContainerRecord
{
    ContainerType type;
    int           lenght;
    int           dataPTR;
};


class DataContainer
{
private:
    bool modfied = false;
    std::vector<ContainerRecord> recordContainer;
    std::vector<uint8_t> dataContainer;
    std::vector<uint8_t> binaryContainer;

    void addData(ContainerType type, void* data, int lenght);

    void writeBinary(std::vector<uint8_t>& outBuffer, void* data, int lenght);
    void readBinary(std::vector<uint8_t>& inBuffer, int startIndex, void* data, int lenght);
    bool checkIndex(std::string mothod, int index);
    bool checkType(std::string mothod, ContainerType type, int index);

public:
    DataContainer(/* args */) = default;
    DataContainer(std::vector<uint8_t>& data);
    DataContainer(uint8_t * buffer,int size);

    ~DataContainer() = default;

    void PushVariable(bool var);
    void PushVariable(int var);
    void PushVariable(std::string var);
    void PushVariable(Vect2 var);
    void PushVariable(DataContainer& var);

    bool            GetBool(int index);
    int             GetInt(int index);
    std::string     GetString(int index);
    Vect2           GetVect2(int index);
    DataContainer   GetDataContainer(int index);

    void                DeSerialize();
    std::vector<uint8_t>& Serialize();
};