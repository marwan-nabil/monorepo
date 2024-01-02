#include <Windows.h>
#include <stdint.h>
#include "sources\win32\base_types.h"
#include "sources\win32\strings\string_list.h"

#include "build.h"
#include "targets\i686-elf\i686-elf_targets.h"
#include "targets\win32\win32_targets.h"
#include "targets\hdl\hdl_targets.h"
#include "target_configuration.h"

build_target_config BuildTargetConfigurations[CONFIGURED_TARGETS_COUNT] =
{
    {"lint", &BuildLint, "[job_per_directory]", NULL, NULL},
    {"fetch_data", &BuildFetchData, NULL, NULL, NULL},
    {"compilation_tests", &BuildCompilationTests, NULL, NULL, NULL},
    {"fat12_tests", &BuildFat12Tests, NULL, NULL, NULL},
    {"simulator", &BuildSimulator, NULL, NULL, NULL},
    {"directx_demo", &BuildDirectxDemo, "[debug, release]", NULL, NULL},
    {"handmade_hero", &BuildHandmadeHero, NULL, NULL, NULL},
    {"imgui_demo", &BuildImguiDemo, "[opengl2, dx11]", NULL, NULL},
    {"ray_tracer", &BuildRayTracer, "[1_lane, 4_lanes, 8_lanes]", NULL, NULL},
    {"os", &BuildOsFloppyDiskImage, NULL, NULL, NULL},
    {"verilog_demo", &BuildVerilogDemo, NULL, NULL, NULL},
    {"uart_app", &BuildUARTApp, NULL, NULL, NULL},
};