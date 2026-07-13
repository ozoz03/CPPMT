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
        // BallisticTable table_;
        table_.load(DATA_DIR_PATH.data() + std::string("/ballistic_table.txt")); // load the table
        std::cout << "TableSolver created with table of size: " << table_.data.size() << std::endl;
    }

    // Метод для обчислення результатів на основі вхідних параметрів
    // BallisticTable::Result solve(float z0, float v0, float m, float d, float l) const;
    BalisticResult solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) override;

private:
    BallisticTable table_; 
};