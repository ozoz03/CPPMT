#pragma once
#include "ITargetProvider.h"
#include <string>
#include "Target.h"


class JsonTargetProvider: public ITargetProvider {
private:
    Target** targets;
    int targetCount;
public:
    JsonTargetProvider(): ITargetProvider(){};
    JsonTargetProvider(const std::string filename) : ITargetProvider(filename){
        this->filename = filename;
        load();
    }
    ~JsonTargetProvider() {
        for (int i = 0; i < targetCount; i++) {
            delete[] targets[i];
        }
        delete[] targets;
    }

    
    int getTargetCount() override {
        return targetCount;
    }
    Target getTarget(int index) override;
    Target** getTargets() override;
    void load() override;
};   