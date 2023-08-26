struct electric_point
{
    f32 CurrentPotential;
    f32 NextPotential;
};

struct electric_wire
{
    electric_point *A;
    electric_point *B;
    u32 InverseEquillibriumRate;
};

void ElectricPointUpdate(electric_point *Point)
{
    Assert(Point);
    Point->CurrentPotential = Point->NextPotential;
}

void WireUpdate(electric_wire *Wire)
{
    Assert(Wire);
    Assert(Wire->InverseEquillibriumRate);

    ElectricPointUpdate(Wire->A);
    ElectricPointUpdate(Wire->B);

    electric_point *LowerPotentialPoint;
    electric_point *HigherPotentialPoint;

    if (Wire->A->CurrentPotential > Wire->B->CurrentPotential)
    {
        LowerPotentialPoint = Wire->B;
        HigherPotentialPoint = Wire->A;
    }
    else
    {
        LowerPotentialPoint = Wire->A;
        HigherPotentialPoint = Wire->B;
    }

    f32 PotentialDifference = HigherPotentialPoint->CurrentPotential - LowerPotentialPoint->CurrentPotential;
    f32 StepSize = PotentialDifference / Wire->InverseEquillibriumRate;
    HigherPotentialPoint->NextPotential -= StepSize;
    LowerPotentialPoint->NextPotential += StepSize;
}