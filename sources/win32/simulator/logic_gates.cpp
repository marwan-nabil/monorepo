void AndGateUpdate(logic_gate_2_1 *And)
{
    if
    (
        (And->B->CurrentCharge > And->LogicThreshold) &&
        (And->A->CurrentCharge > And->LogicThreshold)
    )
    {
        And->Q->NextCharge = And->LogicHigh;
    }
    else
    {
        And->Q->NextCharge = And->LogicLow;
    }
}

void OrGateUpdate(logic_gate_2_1 *Or)
{
    if
    (
        (Or->B->CurrentCharge > Or->LogicThreshold) ||
        (Or->A->CurrentCharge > Or->LogicThreshold)
    )
    {
        Or->Q->NextCharge = Or->LogicHigh;
    }
    else
    {
        Or->Q->NextCharge = Or->LogicLow;
    }
}

void XorGateUpdate(logic_gate_2_1 *Xor)
{
    if
    (
        (Xor->B->CurrentCharge > Xor->LogicThreshold) ^
        (Xor->A->CurrentCharge > Xor->LogicThreshold)
    )
    {
        Xor->Q->NextCharge = Xor->LogicHigh;
    }
    else
    {
        Xor->Q->NextCharge = Xor->LogicLow;
    }
}