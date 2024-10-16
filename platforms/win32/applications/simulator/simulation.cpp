#include <Windows.h>
#include <stdint.h>
#include <math.h>

#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\basic_defines.h"
#include "platforms\win32\libraries\math\integers.h"
#include "platforms\win32\libraries\math\floats.h"
#include "platforms\win32\libraries\math\scalar_conversions.h"
#include "platforms\win32\libraries\math\vector2.h"
#include "platforms\win32\libraries\math\vector3.h"
#include "platforms\win32\libraries\math\vector4.h"
#include "platforms\win32\libraries\math\rectangle2.h"
#include "platforms\win32\libraries\rasterizer\rasterizer.h"

#include "electrical.h"
#include "logic_gates.h"
#include "internal_types.h"
#include "simulation.h"

void ConductorUpdate(conductor *Wire, f32 TimeDelta)
{
    electric_point *HigherChargePoint;
    electric_point *LowerChargePoint;

    // if (Wire->A->CurrentCharge > Wire->B->CurrentCharge)
    // {
        HigherChargePoint = Wire->A;
        LowerChargePoint = Wire->B;
    // }
    // else
    // {
    //     HigherChargePoint = Wire->B;
    //     LowerChargePoint = Wire->A;
    // }

    f32 ChargeDifference = HigherChargePoint->CurrentCharge - LowerChargePoint->CurrentCharge;
    f32 Current = ChargeDifference * Wire->Conductivity;
    f32 ChargeMovement = Current * TimeDelta;

    HigherChargePoint->NextCharge = HigherChargePoint->CurrentCharge - ChargeMovement;
    LowerChargePoint->NextCharge = LowerChargePoint->CurrentCharge + ChargeMovement;
}

void CapacitorUpdate(capacitor *Capacitor, f32 TimeDelta)
{
    f32 ChargeDifferenceForceA = Capacitor->Factor0 * (Capacitor->A->CurrentCharge - Capacitor->CurrentChargePlateA);
    f32 ChargeDifferenceForceB = Capacitor->Factor0 * (Capacitor->B->CurrentCharge - Capacitor->CurrentChargePlateB);

    f32 OtherPlateChargeDifferenceForceA =
        Capacitor->Factor1 * (Capacitor->CurrentChargePlateB - Capacitor->CurrentChargePlateA);
    f32 OtherPlateChargeDifferenceForceB =
        Capacitor->Factor1 * (Capacitor->CurrentChargePlateA - Capacitor->CurrentChargePlateB);

    f32 CurrentA = (ChargeDifferenceForceA - OtherPlateChargeDifferenceForceA) * Capacitor->Capacitance;
    f32 CurrentB = (ChargeDifferenceForceB - OtherPlateChargeDifferenceForceB) * Capacitor->Capacitance;

    f32 ChargeMovementA = CurrentA * TimeDelta;
    f32 ChargeMovementB = CurrentB * TimeDelta;

    Capacitor->A->NextCharge = Capacitor->A->CurrentCharge - ChargeMovementA;
    Capacitor->B->NextCharge = Capacitor->B->CurrentCharge - ChargeMovementB;

    Capacitor->NextChargePlateA = Capacitor->CurrentChargePlateA + ChargeMovementA;
    Capacitor->NextChargePlateB = Capacitor->CurrentChargePlateB + ChargeMovementB;
}

void UpdateElementStates(simulation_state *SimulationState)
{
    for (u32 PointIndex = 0; PointIndex < ArrayCount(SimulationState->Points); PointIndex++)
    {
        SimulationState->Points[PointIndex].CurrentCharge =
            SimulationState->Points[PointIndex].NextCharge;
    }

    for (u32 CapacitorIndex = 0; CapacitorIndex < ArrayCount(SimulationState->Capacitors); CapacitorIndex++)
    {
        SimulationState->Capacitors[CapacitorIndex].CurrentChargePlateA =
            SimulationState->Capacitors[CapacitorIndex].NextChargePlateA;
        SimulationState->Capacitors[CapacitorIndex].CurrentChargePlateB =
            SimulationState->Capacitors[CapacitorIndex].NextChargePlateB;
    }
}

void InitializeSimulationState(simulation_state *SimulationState)
{
    SimulationState->A0.NextCharge = 4000;
    SimulationState->A1.NextCharge = 0;

    SimulationState->B0.NextCharge = -3000;
    SimulationState->B1.NextCharge = 4000;

    SimulationState->C0.NextCharge = 0;
    SimulationState->C1.NextCharge = 0;

    SimulationState->WireA.Conductivity = 0.1f;
    SimulationState->WireB.Conductivity = 0.1f;
    SimulationState->WireC.Conductivity = 0.8f;

    SimulationState->Cap0.Capacitance = 1;
    SimulationState->Cap0.A = &SimulationState->A0;
    SimulationState->Cap0.B = &SimulationState->A1;
}

void UpdateSimulation(f32 TimeDelta, user_input *UserInput, simulation_state *SimulationState)
{
    UpdateElementStates(SimulationState);

    SimulationState->Up = UserInput->Up.IsDown;
    SimulationState->Down = UserInput->Down.IsDown;
    SimulationState->Left = UserInput->Left.IsDown;
    SimulationState->Right = UserInput->Right.IsDown;
    SimulationState->Shift = UserInput->Shift.IsDown;

    CapacitorUpdate(&SimulationState->Cap0, TimeDelta);
}