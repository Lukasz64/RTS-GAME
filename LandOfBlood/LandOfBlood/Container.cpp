#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Utils.h"
#include "Container.h"

//SAFE/UNSAFE
#define CONTINER_MODE SAFE

using namespace std;

string TypeToString(ContainerType type) {
    string str[] = { "bool","int","string","vect2","container" };
    return str[(int)type];
}

DataContainer::DataContainer(vector<uint8_t>& data) {
    binaryContainer = data;
    DeSerialize();
}
DataContainer::DataContainer(uint8_t * buffer,int size){
    for (int i = 0; i < size; i++)
        binaryContainer.push_back(buffer[i]);
    DeSerialize();
}


void DataContainer::writeBinary(vector<uint8_t>& outBuffer, void* data, int lenght) {
    for (int i = 0; i < lenght; i++)
        outBuffer.push_back(((uint8_t*)data)[i]);
}
void DataContainer::readBinary(vector<uint8_t>& inBuffer, int startIndex, void* data, int lenght) {
    #if CONTINER_MODE == SAFE
        if(((size_t)(startIndex+lenght)) > inBuffer.size()){
            ReportError("Container corrutpion error, index-out-of-range(readBinary)");
            return;
        }
    #endif
    
    for (int i = 0; i < lenght; i++)
        ((uint8_t*)data)[i] = inBuffer[startIndex + i];
}
void DataContainer::addData(ContainerType type, void* data, int lenght) {
    ContainerRecord rec{ type,(uint16_t)lenght,(uint16_t)dataContainer.size() };
    recordContainer.push_back(rec);
    writeBinary(dataContainer, data, lenght);
}
void DataContainer::PushVariable(bool var) {
    modfied = true;
    addData(BOOL_, &var, 1);
}
void DataContainer::PushVariable(int var) {
    modfied = true;
    addData(INT_, &var, sizeof(var));
}
void DataContainer::PushVariable(string var) {
    modfied = true;
    addData(STRING_, &var[0], var.length());
}
void DataContainer::PushVariable(Vect2 var) {
    modfied = true;
    addData(VECT2_, &var, sizeof(var));
}
void DataContainer::PushVariable(DataContainer& var) {
    modfied = true;
    vector<uint8_t> data = var.Serialize();
    addData(CONTAINER_, &data[0], data.size());
}
bool DataContainer::checkIndex(string mothod, int index) {
    if ((size_t)index >= recordContainer.size()) {
        ReportError("DataContainer::" + mothod + " -> index out of avelivable");
        return false;
    }
    return true;
}
bool DataContainer::checkType(string mothod, ContainerType type, int index) {
    if (recordContainer[index].type != type) {
        ReportError("DataContainer::" + mothod + " -> type not math reached:" + TypeToString(recordContainer[index].type));
        return false;
    }
    return true;
}

bool DataContainer::GetBool(int index) {
    bool res = false;

    if (!checkIndex("GetBool", index))
        return res;
    if (!checkType("GetBool", BOOL_, index))
        return res;

    readBinary(dataContainer, recordContainer[index].dataPTR, &res, recordContainer[index].lenght);
    return res;
}
int  DataContainer::GetInt(int index) {
    int res = 0;

    if (!checkIndex("GetInt", index))
        return res;
    if (!checkType("GetInt", INT_, index))
        return res;

    readBinary(dataContainer, recordContainer[index].dataPTR, &res, recordContainer[index].lenght);
    return res;
}
string DataContainer::GetString(int index) {
    string res = "";

    if (!checkIndex("GetString", index))
        return res;
    if (!checkType("GetString", STRING_, index))
        return res;

    res = string(recordContainer[index].lenght, '\0');

    readBinary(dataContainer, recordContainer[index].dataPTR, &res[0], recordContainer[index].lenght);
    return res;
}
Vect2  DataContainer::GetVect2(int index) {
    Vect2 res;

    if (!checkIndex("GetVect2", index))
        return res;
    if (!checkType("GetVect2", VECT2_, index))
        return res;

    readBinary(dataContainer, recordContainer[index].dataPTR, &res, recordContainer[index].lenght);
    return res;
}
DataContainer  DataContainer::GetDataContainer(int index) {
    DataContainer res;

    if (!checkIndex("GetDataContainer", index))
        return res;
    if (!checkType("GetDataContainer", CONTAINER_, index))
        return res;

    //res.writeBinary(res.binaryContainer, &dataContainer[recordContainer[index].dataPTR], recordContainer[index].lenght);
    if(recordContainer[index].lenght > 0){
        #if CONTINER_MODE == SAFE
            if(recordContainer[index].lenght > 0 && (recordContainer[index].dataPTR + recordContainer[index].lenght) <= dataContainer.size()){         
        #endif    
                res.binaryContainer.insert(
                    std::end(res.binaryContainer),
                    std::next(dataContainer.begin(), recordContainer[index].dataPTR),
                    std::next(dataContainer.begin(), recordContainer[index].dataPTR + recordContainer[index].lenght)
                );
                res.DeSerialize();        
        #if CONTINER_MODE == SAFE
            } else {
                cout <<recordContainer[index].dataPTR << "," <<recordContainer[index].lenght << "," << dataContainer.size() << endl;
                ReportError("Container corrutpion error, index-out-of-range(GetDataContainer)");
            }
        #endif
    } 
    return res;
}

vector<uint8_t>& DataContainer::Serialize() {
    //cahe
    if (modfied) {
        binaryContainer.clear();
        int i = recordContainer.size();
        writeBinary(binaryContainer, &i, sizeof(int));
        for (ContainerRecord r : recordContainer) {
            writeBinary(binaryContainer, &r, sizeof(ContainerRecord));
        }
        binaryContainer.insert(std::end(binaryContainer), std::begin(dataContainer), std::end(dataContainer));
        modfied = false;
    }
    return binaryContainer;
}
void DataContainer::DeSerialize() {
    dataContainer.clear();
    recordContainer.clear();

    int count = 0;
    ContainerRecord record;
    //load record count
    readBinary(binaryContainer, 0, &count, sizeof(count));
    for (int i = 0; i < count; i++)
    {
        readBinary(binaryContainer, sizeof(count) + (sizeof(record) * i), &record, sizeof(record));
        recordContainer.push_back(record);
    }
    //loda binary data
    int binarIdx = sizeof(count) + (sizeof(record) * count);
    
    #if CONTINER_MODE == SAFE
        if((size_t)binarIdx >= binaryContainer.size()){
            ReportError("Container corrutpion error, index-out-of-range(DeSerialize)");
            return;
        }
    #endif

    dataContainer.insert(std::end(dataContainer), std::next(binaryContainer.begin(), binarIdx), std::end(binaryContainer));
}