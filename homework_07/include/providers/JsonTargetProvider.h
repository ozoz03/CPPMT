#pragma once
#include "ITargetProvider.h"
#include <string>
#include <vector>
#include "Target.h"


class JsonTargetProvider: public ITargetProvider {
private:
    std::vector<Target> targets;
    int targetCount;
public:
    JsonTargetProvider(): ITargetProvider(){};
    JsonTargetProvider(const std::string filename) : ITargetProvider(filename){
        this->filename = filename;
        load();
    }
    ~JsonTargetProvider() {
        std::vector<Target>().swap(targets);
    }

    
    int getTargetCount() override {
        return targetCount;
    }
    Target& getTarget(int index) override;
    std::vector<Target>& getTargets() override;
    void load() override;
};   