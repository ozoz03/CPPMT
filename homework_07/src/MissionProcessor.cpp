

Point Mission::computeDrop(int currentStepIndex, const MissionConfig& cfg) {
    std::cout << "Computing drop for target " << solver->getCurrentTargetIndex() << " at step " << currentStepIndex << std::endl;
    return solver->solve(currentStepIndex, targetProvider->getTargets(), cfg, currentTime, bomb );
};

bool Mission::hasNext() {
        cycleCount++;
        // add time step and increment cycle count
		currentTime += cfg.simTimeStep;
        std::cout << "Cycle " << cycleCount << ": Checking if mission has next step..." << std::endl;
        
        // check if current target is hit
		if (solver->getCurrentDistance() <= cfg.hitRadius) {
			std::cout << "Target " << solver->getCurrentTargetIndex() << " is hit!" << std::endl;
			std::cout << "Simulation complete. Steps: " << cycleCount << std::endl;
			return false; // mission complete
		}
        std::cout << "Checking next target " << solver->getCurrentTargetIndex() << ", step " << currentStepIndex << std::endl;
        return currentStepIndex < targetProvider->getTargetCount(); 
};

void Mission::step()  {
        std::cout << "Processing target " << solver->getCurrentTargetIndex() << std::endl;
        // Simulate the step and calculate distance to target
        std::cout << "Calculating drop for target " << solver->getCurrentTargetIndex() << " at step " << currentStepIndex << std::endl;
        currentStepIndex++;
        Point dropPoint = computeDrop(currentStepIndex, cfg);
        std::cout << "Computed drop point: (" << dropPoint.x << ", " << dropPoint.y << ")" << std::endl;
};