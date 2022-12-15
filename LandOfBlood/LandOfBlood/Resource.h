#pragma once
#include <string>

    enum ResourceType
    {
        Wood = 0,
        Stone = 1,
        Gold = 2,
        Food = 3
    };
    class Resource {
    private:
        unsigned int resource[4] = { 0 };
    public:
        Resource() {};
        Resource(Resource & res, int mutiplier);
        Resource(int wood, int stone, int gold, int food);


        void addResource(Resource res);
        bool canSubstract(Resource res);
        bool subResource(Resource res);

        void setResource(ResourceType id, int value);
        int getResource(ResourceType id);

        Resource GetDevidedCopy(int devided);

        std::string ToString();


        void PrintResources();
    };
