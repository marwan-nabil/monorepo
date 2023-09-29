#ifdef ENABLE_ASSERTIONS

#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}
#define AssertIsBit(Value) Assert(!((Value) & (~1ull)))
#define AssertFits(Value, FittingMask) Assert(!((Value) & (~(FittingMask))))
#define InvalidCodepath Assert(!"InvalidCodepath")

#else

#define Assert(Expression)
#define AssertIsBit(Value)
#define AssertFits(Value, FittingMask)
#define InvalidCodepath

#endif // ENABLE_ASSERTIONS