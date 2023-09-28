#ifdef ENABLE_ASSERTIONS

inline void
AssertGoodMask(u32_lane Mask, u32 FaultIndex)
{
    if ((Mask != 0xFFFFFFFF) && (Mask != 0))
    {
        printf("\n ============== something bad happened at %d ============== \n", FaultIndex);
        printf("Mask == %d", Mask);
        fflush(stdout);
        Assert(0);
    }
}

#else

#define AssertGoodMask(Mask, FaultIndex)

#endif // ENABLE_ASSERTIONS