#include <Windows.h>
#include <stdint.h>
#include <math.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\math\integers.h"
#include "sources\win32\libraries\math\floats.h"
#include "sources\win32\libraries\math\scalar_conversions.h"
#include "sources\win32\libraries\math\vector2.h"
#include "sources\win32\libraries\math\vector3.h"
#include "sources\win32\libraries\math\vector4.h"
#include "sources\win32\libraries\math\rectangle2.h"
#include "sources\win32\rasterizer\rasterizer.h"

#include "electrical.h"
#include "logic_gates.h"
#include "internal_types.h"
#include "simulation.h"

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