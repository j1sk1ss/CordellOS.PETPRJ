from pathlib import Path

from SCons.Variables import *
from SCons.Environment import *
from SCons.Node import *

from build_scripts.utility import parse_size, remove_suffix

VARS = Variables('build_scripts/config.py', ARGUMENTS)
VARS.AddVariables(
    EnumVariable("config",
                 help="Build configuration",
                 default="debug",
                 allowed_values=("debug", "release")),

    EnumVariable("arch", 
                 help="Target architecture", 
                 default="i686",
                 allowed_values=("i686")),

    EnumVariable("image_type",
                 help="Type of image",
                 default="disk",
                 allowed_values=("floppy", "disk")),

    EnumVariable("image_file_system",
                 help="Type of image",
                 default="fat32",
                 allowed_values=("fat12", "fat16", "fat32", "ext2"))    
    )

VARS.Add("image_size", 
         help="The size of the image, will be rounded up to the nearest multiple of 512. " +
              "You can use suffixes (k/m/g). " +
              "For floppies, the size is fixed to 1.44MB.",
         default="250m",
         converter=parse_size)

VARS.Add("tool_chain", 
         help="Path to tool_chain directory.",
         default="tool_chain")

DEPS = {
    'binutils': '2.37',
    'gcc': '11.2.0'
}


#
# ***  Host environment ***
#

HOST_ENVIRONMENT = Environment(variables=VARS,
    ENV = os.environ,
    AS = 'nasm',
    CFLAGS = ['-std=c99'],
    CXXFLAGS = ['-std=c++17'],
    CCFLAGS = ['-g'],
    STRIP = 'strip',
)

HOST_ENVIRONMENT.Append(
    PROJECTDIR = HOST_ENVIRONMENT.Dir('.').srcnode()
)

if HOST_ENVIRONMENT['config'] == 'debug':
    HOST_ENVIRONMENT.Append(CCFLAGS = ['-O0'])
else:
    HOST_ENVIRONMENT.Append(CCFLAGS = ['-O3'])

if HOST_ENVIRONMENT['image_type'] == 'floppy':
    HOST_ENVIRONMENT['image_file_system'] = 'fat12'

HOST_ENVIRONMENT.Replace(ASCOMSTR        = "Assembling [$SOURCE]",
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
# ***  Target environment ***
#

platform_prefix = ''
if HOST_ENVIRONMENT['arch'] == 'i686':
    platform_prefix = 'i686-elf-'

tool_chainDir = Path(HOST_ENVIRONMENT['tool_chain'], remove_suffix(platform_prefix, '-')).resolve()
tool_chainBin = Path(tool_chainDir, 'bin')
tool_chainGccLibs = Path(tool_chainDir, 'lib', 'gcc', remove_suffix(platform_prefix, '-'), DEPS['gcc'])

TARGET_ENVIRONMENT = HOST_ENVIRONMENT.Clone(
    AR      = f'{platform_prefix}ar',
    CC      = f'{platform_prefix}gcc',
    CXX     = f'{platform_prefix}g++',
    LD      = f'{platform_prefix}g++',
    RANLIB  = f'{platform_prefix}ranlib',
    STRIP   = f'{platform_prefix}strip',

    # toolchain
    TOOLCHAIN_PREFIX = str(tool_chainDir),
    TOOLCHAIN_LIBGCC = str(tool_chainGccLibs),

    BINUTILS_URL    = f'https://ftp.gnu.org/gnu/binutils/binutils-{DEPS["binutils"]}.tar.xz',
    GCC_URL         = f'https://ftp.gnu.org/gnu/gcc/gcc-{DEPS["gcc"]}/gcc-{DEPS["gcc"]}.tar.xz',
)


TARGET_ENVIRONMENT.Append(
    ASFLAGS = [
        '-f', 'elf',
        '-g'
    ],

    CCFLAGS = [
        '-ffreestanding',
        '-nostdlib'
    ],

    CXXFLAGS = [
        '-fno-exceptions',
        '-fno-rtti',
    ],

    LINKFLAGS = [
        '-nostdlib'
    ],
    
    LIBS    = ['gcc'],
    LIBPATH = [ str(tool_chainGccLibs) ],
)

TARGET_ENVIRONMENT['ENV']['PATH'] += os.pathsep + str(tool_chainBin)

Help(VARS.GenerateHelpText(HOST_ENVIRONMENT))
Export('HOST_ENVIRONMENT')
Export('TARGET_ENVIRONMENT')

variantDir = 'build/{0}_{1}'.format(TARGET_ENVIRONMENT['arch'], TARGET_ENVIRONMENT['config'])
variantDirStage1 = variantDir + '/fst_stg_{0}'.format(TARGET_ENVIRONMENT['image_file_system'])

SConscript('src/libs/core/SConscript', variant_dir=variantDir + '/libs/core', duplicate=0)
SConscript('src/boot_loader/fst_stg/SConscript', variant_dir=variantDirStage1, duplicate=0)
SConscript('src/boot_loader/sec_stg/SConscript', variant_dir=variantDir + '/sec_stg', duplicate=0)
SConscript('src/kernel/SConscript', variant_dir=variantDir + '/kernel', duplicate=0)
SConscript('image/SConscript', variant_dir=variantDir, duplicate=0)

Import('fst_stg', 'sec_stg', 'kernel', 'image')
Default(fst_stg, sec_stg, kernel, image)