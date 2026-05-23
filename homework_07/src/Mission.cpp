

class Mission : public IMission {
    public:
    void init(const ConfigSource& configSource) override {
        // load config
    }

    bool hasNext() override {
        // check if there are more targets
    }

    void reset() override {
        // reset mission state
    }

    void changeSolver(const Solver& s) override {
        // change the solver used for calculations
    }
    
    ~Mission() override {}
};