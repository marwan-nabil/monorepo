#pragma once

b32 BuildBootSectorImage(build_context *BuildContext);
b32 BuildBootloaderImage(build_context *BuildContext);
b32 BuildKernelImage(build_context *BuildContext);
b32 BuildOsFloppyDiskImage(build_context *BuildContext);