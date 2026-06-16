#pragma once
#include <string>
#include <vector>

struct Target;

class ITargetProvider {
public:
    ITargetProvider() = default;
    ITargetProvider(const std::string filename): filename(filename) {};
    virtual int getTargetCount() = 0;
    virtual Target& getTarget(int index) = 0;
    virtual std::vector<Target>& getTargets() = 0;
    virtual ~ITargetProvider() {};
    protected:
        std::string filename;   
    private:
        virtual void load() = 0;     
};

