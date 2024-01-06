#include <stdint.h>

#include "ata.h"
#include "stdio.h"


#define GRUB_ISO9660_FSTYPE_DIR		0040000
#define GRUB_ISO9660_FSTYPE_REG		0100000
#define GRUB_ISO9660_FSTYPE_SYMLINK	0120000
#define GRUB_ISO9660_FSTYPE_MASK	0170000

#define GRUB_ISO9660_LOG2_BLKSZ		2
#define GRUB_ISO9660_BLKSZ		2048

#define GRUB_ISO9660_RR_DOT		2
#define GRUB_ISO9660_RR_DOTDOT		4

#define GRUB_ISO9660_VOLDESC_BOOT	0
#define GRUB_ISO9660_VOLDESC_PRIMARY	1
#define GRUB_ISO9660_VOLDESC_SUPP	2
#define GRUB_ISO9660_VOLDESC_PART	3
#define GRUB_ISO9660_VOLDESC_END	255

/* The head of a volume descriptor.  */
struct grub_iso9660_voldesc
{
  uint8_t type;
  uint8_t magic[5];
  uint8_t version;
} __attribute__ ((packed));

/* A directory entry.  */
struct grub_iso9660_dir
{
  uint8_t len;
  uint8_t ext_sectors;
  uint32_t first_sector;
  uint32_t first_sector_be;
  uint32_t size;
  uint32_t size_be;
  uint8_t unused1[7];
  uint8_t flags;
  uint8_t unused2[6];
  uint8_t namelen;
} __attribute__ ((packed));

struct grub_iso9660_date
{
  uint8_t year[4];
  uint8_t month[2];
  uint8_t day[2];
  uint8_t hour[2];
  uint8_t minute[2];
  uint8_t second[2];
  uint8_t hundredth[2];
  uint8_t offset;
} __attribute__ ((packed));

/* The primary volume descriptor.  Only little endian is used.  */
struct grub_iso9660_primary_voldesc
{
  struct grub_iso9660_voldesc voldesc;
  uint8_t unused1[33];
  uint8_t volname[32];
  uint8_t unused2[16];
  uint8_t escape[32];
  uint8_t unused3[12];
  uint32_t path_table_size;
  uint8_t unused4[4];
  uint32_t path_table;
  uint8_t unused5[12];
  struct grub_iso9660_dir rootdir;
  uint8_t unused6[624];
  struct grub_iso9660_date created;
  struct grub_iso9660_date modified;
} __attribute__ ((packed));

/* A single entry in the path table.  */
struct grub_iso9660_path
{
  uint8_t len;
  uint8_t sectors;
  uint32_t first_sector;
  uint16_t parentdir;
  uint8_t name[0];
} __attribute__ ((packed));

/* An entry in the System Usage area of the directory entry.  */
struct grub_iso9660_susp_entry
{
  uint8_t sig[2];
  uint8_t len;
  uint8_t version;
  uint8_t data[0];
} __attribute__ ((packed));

/* The CE entry.  This is used to describe the next block where data
   can be found.  */
struct grub_iso9660_susp_ce
{
  struct grub_iso9660_susp_entry entry;
  uint32_t blk;
  uint32_t blk_be;
  uint32_t off;
  uint32_t off_be;
  uint32_t len;
  uint32_t len_be;
} __attribute__ ((packed));

// struct grub_iso9660_data
// {
//   struct grub_iso9660_primary_voldesc voldesc;
//   grub_disk_t disk;
//   unsigned int first_sector;
//   int rockridge;
//   int susp_skip;
//   int joliet;
// };

struct grub_fshelp_node
{
  struct grub_iso9660_data *data;
  unsigned int size;
  unsigned int blk;
  unsigned int dir_blk;
  unsigned int dir_off;
};

// static grub_dl_t my_mod;