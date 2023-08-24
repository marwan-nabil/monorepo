void UpdateSimulation(f32 TimeDelta, user_input_sample *CurrentUserInput, simulation_state *SimulationState)
{
    SimulationState->Up = CurrentUserInput->Up.IsDown;
    SimulationState->Down = CurrentUserInput->Down.IsDown;
    SimulationState->Left = CurrentUserInput->Left.IsDown;
    SimulationState->Right = CurrentUserInput->Right.IsDown;
}