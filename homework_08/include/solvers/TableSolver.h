#pragma once
#include "BalisticResult.h"
#include "BallisticTable.h"
#include "IBallisticSolver.h"
#include "MissionContext.h"
#include "Target.h"
#include <iostream>
#include "config.hpp"
#include <cstring>

class TableSolver: public IBallisticSolver {
public:
    TableSolver() { 
        table_.load(DATA_DIR_PATH.data() + std::string("/ballistic_table.txt")); // load the table
        std::cout << "TableSolver created with table of size: " << table_.data.size() << std::endl;
    }

    BalisticResult solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) override;

private:
    BallisticTable table_; 
};