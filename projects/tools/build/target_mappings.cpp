target_mapping BuildTargetMappings[] =
{
    {"lint", &BuildLint},
    {"fetch_data", &BuildFetchData},
    {"compilation_tests", &BuildCompilationTests},
    {"fat12_tests", &BuildFat12Tests},
    {"simulator", &BuildSimulator},
    {"directx_demo", &BuildDirectxDemo},
    {"handmade_hero", &BuildHandmadeHero},
    {"imgui_demo", &BuildImguiDemo},
    {"ray_tracer", &BuildRayTracer},
    {"x86_kernel", &Buildx86Kernel},
    {"x86_kernel_tests", &BuildX86KernelTests},
};