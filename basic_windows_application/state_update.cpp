void InitializeSimulationState(simulation_state *SimulationState)
{
    SimulationState->SelectedDataPointIndex = 0;
    SimulationState->DataPointCount = 10;
    SimulationState->DataValues = (f32 *)malloc(SimulationState->DataPointCount * sizeof(f32));
    SimulationState->DataColors = (v4 *)malloc(SimulationState->DataPointCount * sizeof(v4));

    for (u32 DataPointIndex = 0; DataPointIndex < SimulationState->DataPointCount; DataPointIndex++)
    {
        SimulationState->DataValues[DataPointIndex] = 0;
        SimulationState->DataColors[DataPointIndex] = V4(0, 0, 0, 1.0f);
    }
}

void UpdateSimulation(f32 TimeDelta, user_input *UserInput, simulation_state *SimulationState)
{
    f32 ChangeRate = (SimulationState->DoubleSpeed + 1.0f) * TimeDelta;

    if (UserInput->Number)
    {
        SimulationState->SelectedDataPointIndex = UserInput->Number - '0';
    }

    if (UserInput->Shift)
    {
        SimulationState->DoubleSpeed = 1;
    }
    else
    {
        SimulationState->DoubleSpeed = 0;
    }

    if (UserInput->Up)
    {
        SimulationState->DataValues[SimulationState->SelectedDataPointIndex] += ChangeRate * 400.0f;
    }
    if (UserInput->Down)
    {
        SimulationState->DataValues[SimulationState->SelectedDataPointIndex] -= ChangeRate * 400.0f;
    }
    if (UserInput->Right)
    {
        v3 OriginalColor = SimulationState->DataColors[SimulationState->SelectedDataPointIndex].XYZ;
        OriginalColor.X += ChangeRate * 312.5f;
        OriginalColor.Y += ChangeRate * 312.5f;
        OriginalColor.Z += ChangeRate * 312.5f;

        Clamp(OriginalColor.X, 0, 1.0f);
        Clamp(OriginalColor.Y, 0, 1.0f);
        Clamp(OriginalColor.Z, 0, 1.0f);

        SimulationState->DataColors[SimulationState->SelectedDataPointIndex].XYZ = OriginalColor;
    }
    if (UserInput->Left)
    {
        v3 OriginalColor = SimulationState->DataColors[SimulationState->SelectedDataPointIndex].XYZ;
        OriginalColor.X -= ChangeRate * 312.5f;
        OriginalColor.Y -= ChangeRate * 312.5f;
        OriginalColor.Z -= ChangeRate * 312.5f;

        Clamp(OriginalColor.X, 0, 1.0f);
        Clamp(OriginalColor.Y, 0, 1.0f);
        Clamp(OriginalColor.Z, 0, 1.0f);

        SimulationState->DataColors[SimulationState->SelectedDataPointIndex].XYZ = OriginalColor;
    }

    SimulationState->Up = UserInput->Up;
    SimulationState->Down = UserInput->Down;
    SimulationState->Left = UserInput->Left;
    SimulationState->Right = UserInput->Right;
}