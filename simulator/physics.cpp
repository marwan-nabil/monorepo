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

    potential PotentialDifference = HigherPotentialPoint->CurrentPotential - LowerPotentialPoint->CurrentPotential;
    f32 StepSize = PotentialDifference / Wire->InverseEquillibriumRate;
    HigherPotentialPoint->NextPotential -= StepSize;
    LowerPotentialPoint->NextPotential += StepSize;
}