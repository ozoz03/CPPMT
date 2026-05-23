#pragma once

class IMission {
    public:
    virtual void init(const ConfigSource& configSource) = 0;
    virtual bool hasNext() = 0;
    virtual void step() = 0;
    virtual void reset() = 0;
    virtual void changeSolver(const Solver& s) = 0;

    virtual ~IMission(){};
};