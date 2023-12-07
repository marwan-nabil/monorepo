inline controller_state *
GetController(game_input *GameInput, i32 ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(GameInput->ControllerStates));
    return &GameInput->ControllerStates[ControllerIndex];
}