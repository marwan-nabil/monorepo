#include <Windows.h>
#include <stdint.h>

#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\strings\string_list.h"
#include "platforms\win32\tools\build\actions\build_context.h"
#include "platforms\win32\tools\build\targets.h"
#include "platforms\win32\tools\build\build.h"
#include "platforms\win32\build.h"
#include "platforms\hdl\build.h"
#include "platforms\i686-elf\build.h"

build_target_config BuildTargetConfigurations[CONFIGURED_TARGETS_COUNT] =
{
    {"lint", &BuildLint, "[job_per_directory]", NULL, NULL},
    {"fetch_data", &BuildFetchData, NULL, NULL, NULL},
    {"build_tests", &BuildBuildTests, NULL, NULL, NULL},
    {"fat12_tests", &BuildFat12Tests, NULL, NULL, NULL},
    {"simulator", &BuildSimulator, NULL, NULL, NULL},
    {"directx_demo", &BuildDirectxDemo, NULL, NULL, NULL},
    {"handmade_hero", &BuildHandmadeHero, NULL, NULL, NULL},
    {"imgui_demo", &BuildImguiDemo, "[opengl2, dx11]", NULL, NULL},
    {"ray_tracer", &BuildRayTracer, "[1_lane, 4_lanes, 8_lanes]", NULL, NULL},
    {"os", &BuildOsFloppyDiskImage, NULL, NULL, NULL},
    {"verilog_demo", &BuildVerilogDemo, NULL, NULL, NULL},
    {"uart_app", &BuildUARTApp, NULL, NULL, NULL},
    {"refterm", &BuildRefTerm, NULL, NULL, NULL},
};