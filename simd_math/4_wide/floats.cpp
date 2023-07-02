inline f32_lane 
operator/(f32_lane A, f32_lane B)
{
    f32_lane Result = _mm_div_ps(A, B);
    return Result;
}
