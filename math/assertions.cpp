#if ENABLE_ASSERTIONS

#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define AssertIsBit(Value) Assert(!((Value) & (~1ull)))
#define AssertFits(Value, FittingMask) Assert(!((Value) & (~(FittingMask))))

#else

#define Assert(Expression)
#define Assert(Expression)
#define AssertIsBit(Value)
#define AssertFits(Value, FittingMask)

#endif