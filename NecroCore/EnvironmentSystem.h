#pragma once

namespace NecroCore
{
    class Game;
    struct CommandResult;

    class EnvironmentSystem
    {
    public:
        void ApplyTurn(Game& game, CommandResult& result);
    };
}