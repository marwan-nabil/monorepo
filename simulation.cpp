
enum simulation_flags
{
    SF_DONE = (1u << 0u),
};

struct simulation_state
{
    simulation_flags SimulationFlags;
};

void
UpdateSimulation(f32 TimeDelta, simulation_state *SimulationState)
{
    if (SimulationState->SimulationFlags & SF_DONE)
    {
        SimulationState->SimulationFlags = SF_DONE;
    }
}