inline game_controller_state *
GetController(game_input *GameInput, i32 ControllerIndex)
{
    Assert(ControllerIndex < ArrayLength(GameInput->ControllerStates));
    return &GameInput->ControllerStates[ControllerIndex];
}