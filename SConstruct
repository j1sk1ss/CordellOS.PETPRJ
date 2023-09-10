from pathlib import Path

from SCons.Variables import *
from SCons.Environment import *
from SCons.Node import *

from build_scripts.phony_targets import phony_targets
from build_scripts.utility import parse_size, remove_suffix

VARS = Variables('build_scripts/config.py', ARGUMENTS)
VARS.AddVariables(

    # Build config
    EnumVariable("config",
                help="Build configuration",
                default="debug",
                allowed_values=("debug", "release")),

    # OS architecture
    EnumVariable("arch",
                help="Target architecture",
                default="i686",
                allowed_values=("i686")),

    # Image type
    EnumVariable("image_type",
                help="Type of image",
                default="disk",
                allowed_values=("floppy", "disk")),

    # Image file system
    EnumVariable("image_file_system",
                help="Type of image",
                default="fat32",
                allowed_values=("fat12", "fat16", "fat32", "ext2"))

)

VARS.Add("image_size",
        help="The size of image will be roubded, also u can use suffizes like [k/m/g]",
        default="250m",
        converter=parse_size)

VARS.Add("tool_chain",
        help="Path to tool_chain directory",
        default="tool_chain")

DEPS = {
    'binutils': '2.39',
    'gcc': '12.1.0'
}

#
#   *** HOST ENVIROMENT ***
#

HOST_ENVIRONMENT = Environment(variables=VARS,
    ENV      = os.environ,
    AS       = 'nasm',          # ASM compiler
    CFLAGS   = ['-std=c99'],    # C compiler
    CXXFLAGS = ['-std=c++17'],  # C++ compiler
    CCFLAGS  = ['-g'],          # C and C++ compiler
    STRIP    = 'strip'
)

if HOST_ENVIRONMENT['config'] == 'debug':        # Optimisatin status
    HOST_ENVIRONMENT.Append(CCFLAGS = ['-O0'])
else:
    HOST_ENVIRONMENT.Append(CCFLAGS = ['-O3'])

if HOST_ENVIRONMENT['image_type'] == 'floppy':   # If it floppy goes to fat12
    HOST_ENVIRONMENT['image_file_system'] = 'fat12'

HOST_ENVIRONMENT.Replace(ASCOMSTR       = "Assembling [$SOURCE]",   # Messaging
                        CCCOMSTR        = "Compiling  [$SOURCE]",
                        CXXCOMSTR       = "Compiling  [$SOURCE]",
                        FORTRANPPCOMSTR = "Compiling  [$SOURCE]",
                        FORTRANCOMSTR   = "Compiling  [$SOURCE]",
                        SHCCCOMSTR      = "Compiling  [$SOURCE]",
                        SHCXXCOMSTR     = "Compiling  [$SOURCE]",

                        LINKCOMSTR      = "Linking    [$TARGET]",
                        SHLINKCOMSTR    = "Linking    [$TARGET]",
                        INSTALLSTR      = "Installing [$TARGET]",
                        ARCOMSTR        = "Archiving  [$TARGET]",
                        RANLIBCOMSTR    = "Ranlib     [$TARGET]")

#
#   *** Target environment ***
#

platform_prefix = ''
if HOST_ENVIRONMENT['arch'] == 'i686':
    platform_prefix = 'i686-elf-'

tool_chain_directory    = Path(HOST_ENVIRONMENT['tool_chain'], remove_suffix(platform_prefix, '-')).resolve()
tool_chain_bin          = Path(tool_chain_directory, 'bin')
tool_chain_gcc_libs     = Path(tool_chain_directory, 'lib', 'gcc', remove_suffix(platform_prefix, '-'), DEPS['gcc'])

TARGET_ENVIRONMENT = HOST_ENVIRONMENT.Clone(
    AR      = f'{platform_prefix}ar',
    CC      = f'{platform_prefix}gcc',
    CXX     = f'{platform_prefix}g++',
    LD      = f'{platform_prefix}g++',
    RANLIB  = f'{platform_prefix}ranlib',
    STRIP   = f'{platform_prefix}strip',

    # tool_chain
    TOOLCHAIN_PREFIX    = str(tool_chain_directory),
    TOOLCHAIN_LIBGCC    = str(tool_chain_gcc_libs),
    BINUTILS_URL        = f'https://ftp.gnu.org/gnu/binutils/binutils-{DEPS["binutils"]}.tar.xz',
    GCC_URL             = f'https://ftp.gnu.org/gnu/gcc/gcc-{DEPS["gcc"]}/gcc-{DEPS["gcc"]}.tar.xz',
)

TARGET_ENVIRONMENT.Append(
    ASFLAGS = [
        '-f', 'elf',
        '-g'
    ],
    
    CCFLAGS = [
        '-ffreestanding',
        '-nostdlib',
    ],

    CXXFLAGS = [
        '-fno-exceptions',
        '-fno-rtti',
    ],

    LINKFLAGS = [
        '-nostdlib'
    ],

    LIBS = ['gcc'],
    LIBPATH = [ str(tool_chain_gcc_libs) ],
)

TARGET_ENVIRONMENT['ENV']['PATH'] += os.pathsep + str(tool_chain_bin)

Help(VARS.GenerateHelpText(HOST_ENVIRONMENT))
Export('HOST_ENVIRONMENT')
Export('TARGET_ENVIRONMENT')

variant_directory = 'build/{0}_{1}'.format(TARGET_ENVIRONMENT['arch'], TARGET_ENVIRONMENT['config'])
variant_directory_fst_stg = variant_directory + '/fst_stg_{0}'.format(TARGET_ENVIRONMENT['image_file_system'])

SConscript('src/boot_loader/fst_stg/SConscript', variant_dir=variant_directory_fst_stg, duplicate=0)
SConscript('src/boot_loader/sec_stg/SConscript', variant_dir=variant_directory + '/sec_stg', duplicate=0)
SConscript('src/kernel/SConscript', variant_dir=variant_directory + '/kernel', duplicate=0)
SConscript('image/SConscript', variant_dir=variant_directory, duplicate=0)

Import('image')
Default(image)

# Phony targets
phony_targets(HOST_ENVIRONMENT, 
             run=['./scripts/run.sh', HOST_ENVIRONMENT['image_type'], image[0].path],
             debug=['./scripts/debug.sh', HOST_ENVIRONMENT['image_type'], image[0].path],
             toolchain=['./scripts/setup_tool_chain.sh', HOST_ENVIRONMENT['tool_chain']])

Depends('run', image)
Depends('debug', image)