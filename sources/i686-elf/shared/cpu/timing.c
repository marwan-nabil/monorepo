void SpinlockWait(u32 SleepLoops)
{
    SleepLoops *= 1000000;
    for (u32 Index = 0; Index < SleepLoops; Index++) {;}
}