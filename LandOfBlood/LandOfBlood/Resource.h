#pragma once
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
        Resource(int wood, int stone, int gold, int food);
        void setResource(ResourceType id, int value);
        int getResource(ResourceType id);
        void PrintResources();
    };
