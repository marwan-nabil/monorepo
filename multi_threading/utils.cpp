inline u64
LockedAddAndReturnOldValue(volatile u64 *Addend, u64 Value)
{
    return InterlockedExchangeAdd64((volatile LONG64 *)Addend, Value);
}