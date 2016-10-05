#pragma once
#include "game/navigator/NavigatorManager.h"
#include "thread/ExecutorService.h"

class Game
{
public:
    Game();
    ~Game();

    NavigatorManager *getNavigatorManager() { return navigatorManager; }
    ExecutorService *getExecutorService() { return executorService; }

private:
    NavigatorManager *navigatorManager;
    ExecutorService *executorService;
};


