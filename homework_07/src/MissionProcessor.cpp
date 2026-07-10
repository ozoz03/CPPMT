#include "MissionProcessor.h"


bool MissionProcessor::hasNext() {
        // check if current target is hit
		if (this->ctx.droneContext.targetDistance <= this->ctx.cfg.hitRadius) {
			std::cout << "Target " << ctx.currentTargetIndex << " is hit!" << std::endl;
			std::cout << "Simulation complete. Steps: " << this->ctx.currentStepIndex << std::endl;
			return false; // mission complete
		}
        if (this->ctx.currentStepIndex + 1 >= MAX_STEPS) {
            std::cout << "MAX_STEPS reached." << std::endl;
            return false;
        }
        return true; 
};

Point MissionProcessor::computeDrop(MissionContext& ctx) {
        std::cout << "Computing drop for " << targetProvider->getTargets().size() << " targets at step " << ctx.currentStepIndex << std::endl;
        std::cout << "Current time: " << ctx.droneContext.currentTime << std::endl;
        std::cout << "Step " << ctx.currentStepIndex << " pos=(" << ctx.droneContext.dronePos.x << "," << ctx.droneContext.dronePos.y << ")" << std::endl;
		std::cout << "  target=" << ctx.currentTargetIndex << " state=" << ctx.droneContext.droneStateName << std::endl;

    
        int nearestTargetIndex = getNearestTarget(ctx, targetProvider->getTargets());
            
    	if (ctx.currentTargetIndex != nearestTargetIndex) {
	    	ctx.currentTargetIndex = nearestTargetIndex;
		    std::cout << "Switching to target " << ctx.currentTargetIndex << std::endl;
		    // change the context
		    ctx.droneContext.targetIdx = nearestTargetIndex;
		    // ctx.desiredDir = this->targetsToDroneAngleRadians[nearestTargetIndex];
    	}

        BalisticResult result = solver->solve(targetProvider->getTargets(), ctx, bomb);
        
        ctx.droneContext.dropPoint = result.dropPoint;
        ctx.droneContext.aimPoint = result.aimPoint;
        ctx.droneContext.predictedTarget = result.predictedTarget;

        return result.dropPoint;
    };


void MissionProcessor::step()  {

        this->ctx.droneContext.currentTime += this->ctx.cfg.simTimeStep;

        std::cout << "simTimeStep: " << this->ctx.cfg.simTimeStep << std::endl;
        // hitRadius
        std::cout << "hitRadius: " << this->ctx.cfg.hitRadius << std::endl;
        std::cout << "attackSpeed: " << this->ctx.cfg.attackSpeed << std::endl;
        std::cout << "STEP current time: " << this->ctx.droneContext.currentTime << std::endl;

        std::cout << "STEP currentStepIndex: " << this->ctx.currentStepIndex << std::endl;
        // change a context
        if (this->ctx.currentStepIndex > 0) {
            // a new sim step
            SimStep newSimStep = {
                .dronePos = this->ctx.droneContext.dronePos,
                .droneDirection = this->ctx.droneContext.droneDirection,
                .targetDistance = this->ctx.droneContext.targetDistance,
                .droneStateName = this->currentState->name(),
                .droneSpeed = this->ctx.droneContext.droneSpeed,
                .targetIdx = this->ctx.droneContext.targetIdx,
                .dropPoint = this->ctx.droneContext.dropPoint,
                .aimPoint = Point{},    	
	            .predictedTarget = Point{},
                .currentTime = this->ctx.droneContext.currentTime
            };
            this->ctx.droneContext = newSimStep;

            auto next = this->currentState->execute(this->ctx); 
            if (next) this->currentState = std::move(next);
		}

        Point dropPoint = computeDrop(this->ctx);
        std::cout << "Computed drop point: (" << dropPoint.x << ", " << dropPoint.y << ")" << std::endl;
        ctx.droneContext.dropPoint = dropPoint;
        
        this->simSteps[this->ctx.currentStepIndex] = ctx.droneContext;

        this->ctx.currentStepIndex++;
        std::cout << "Current Step Index: " << this->ctx.currentStepIndex << std::endl;
    };    

void MissionProcessor::init(const MissionConfig& cfg, const AmmoParams& bomb) {
        std::cout << "Initializing mission an ammo: " << cfg.ammoName<< std::endl;

        this->simSteps = std::vector<SimStep>(MAX_STEPS);
        SimStep startStep = {cfg.startPos,cfg.initialDir, 0, currentState->name(),-1,0,{0,0},{0,0},{0,0}, 0};
        simSteps[0] = startStep;
       
        this->ctx = {0, 0,startStep, cfg, 0,0};
        this->bomb = bomb;

        std::cout << "Mission initialized with MAX_STEPS: " << MAX_STEPS << std::endl;
    };