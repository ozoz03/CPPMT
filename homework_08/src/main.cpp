#include "MissionProcessor.h"
#include "DronePhysics.h"
#include "ITargetProvider.h"
#include "SolverType.h"
#include "config.hpp"
#include "ComponentFactory.h"
#include "IConfigLoader.h"
#include "StateStopped.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
    try {
        // Завантаження конфігу, створення компонентів через фабрику.
        std::string fileName = DATA_DIR_PATH.data() + std::string("/config.json");
        auto jsonConfigLoader = ConfigLoaderFactory::createConfigLoader(fileName);
        MissionConfig cfg = jsonConfigLoader->getConfig();
        AmmoParams bomb = jsonConfigLoader->getAmmoParams();

        std::string targetsPath = DATA_DIR_PATH.data() + std::string("/targets.json");
        auto provider = TargetProviderFactory::createTargetProvider(
            Source::JSON, targetsPath, cfg.arrayTimeStep, cfg.timeScale);
        auto physics = std::make_unique<DronePhysics>(cfg);

        MissionProcessor mission(
            BallisticSolverFactory::createBallisticSolver(SolverType::TABLE),
            provider.get(), physics.get(), std::make_unique<StateStopped>());
        mission.init(cfg, bomb);

        // Три потоки: цілі, фізика, місія.
        std::thread providerThread(&ITargetProvider::run, provider.get());
        std::thread physicsThread(&DronePhysics::run, physics.get());
        std::thread missionThread(&MissionProcessor::run, &mission);

        // Чекаємо, поки всі потоки створені й готові — лише тоді даємо старт,
        // інакше цілі почнуть рух, поки решта ще ініціалізується (розсинхрон).
        while (!provider->isThreadReady() || !mission.isThreadReady() || !physics->isThreadReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        provider->start();
        physics->start();
        mission.start();

        // Потік місії — єдиний, на якому чекаємо. Він сам вирішує, коли завершити.
        missionThread.join();
        physics->stop();
        provider->stop();
        providerThread.join();
        physicsThread.join();
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }
    return 0;
}
