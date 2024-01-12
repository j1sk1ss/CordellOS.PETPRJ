#include "../../include/fat.h"

//////////////////////////////

	//Global variables
	unsigned int fat_size;
	unsigned int fat_type;

	unsigned int first_fat_sector;
	unsigned int first_data_sector;

	unsigned int total_sectors;
	unsigned int total_clusters;

	unsigned int bytes_per_sector;
	unsigned int sectors_per_cluster;

	unsigned int ext_root_cluster;

	char* current_path;

//////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____   ___   ___ _____   ____  _____ ____ _____ ___  ____  
//  | __ ) / _ \ / _ \_   _| / ___|| ____/ ___|_   _/ _ \|  _ \ 
//  |  _ \| | | | | | || |   \___ \|  _|| |     | || | | | |_) |
//  | |_) | |_| | |_| || |    ___) | |__| |___  | || |_| |  _ < 
//  |____/ \___/ \___/ |_|   |____/|_____\____| |_| \___/|_| \_\
//
// Initializes struct "bootsect" to store critical data from the boot sector of the volume

	int FAT_initialize() {
		current_path = (char*)malloc(4);
		strcpy(current_path, "BOOT");

		uint8_t* cluster_data = ATA_read_sector(0);
		if (cluster_data == NULL) {
			printf("Function FAT_initialize: Error reading the first sector of FAT!\n");
			return -1;
		}

		fat_BS_t* bootstruct = (fat_BS_t*)cluster_data;
		total_sectors = (bootstruct->total_sectors_16 == 0)? bootstruct->total_sectors_32 : bootstruct->total_sectors_16;
		fat_size = (bootstruct->table_size_16 == 0) ? ((fat_extBS_32_t*)(bootstruct->extended_section))->table_size_32 : bootstruct->table_size_16;

		int root_dir_sectors = ((bootstruct->root_entry_count * 32) + (bootstruct->bytes_per_sector - 1)) / bootstruct->bytes_per_sector;
		int data_sectors = total_sectors - (bootstruct->reserved_sector_count + (bootstruct->table_count * fat_size) + root_dir_sectors);

		total_clusters = data_sectors / bootstruct->sectors_per_cluster;
		if (total_clusters == 0) 
			total_clusters = bootstruct->total_sectors_32 / bootstruct->sectors_per_cluster;
		else if (total_clusters < 4085) {
			fat_type = 12;
			first_data_sector = bootstruct->reserved_sector_count + bootstruct->table_count * bootstruct->table_size_16 + (bootstruct->root_entry_count * 32 + bootstruct->bytes_per_sector - 1) / bootstruct->bytes_per_sector; 
		}
		else if (total_clusters < 65525) {
			fat_type = 16;
			first_data_sector = bootstruct->reserved_sector_count + bootstruct->table_count * bootstruct->table_size_16 + (bootstruct->root_entry_count * 32 + bootstruct->bytes_per_sector - 1) / bootstruct->bytes_per_sector;
		}
		else {
			fat_type = 32;
			first_data_sector = bootstruct->reserved_sector_count + bootstruct->table_count * ((fat_extBS_32_t*)(bootstruct->extended_section))->table_size_32;
		}

		sectors_per_cluster = bootstruct->sectors_per_cluster;
		bytes_per_sector    = bootstruct->bytes_per_sector;
		first_fat_sector    = bootstruct->reserved_sector_count;
		ext_root_cluster    = ((fat_extBS_32_t*)(bootstruct->extended_section))->root_cluster;

		free(bootstruct);
		return 0;
	}

	BOOL FAT_cluster_free(unsigned int cluster, int fatType) {
		if ((cluster == FREE_CLUSTER_32 && fat_type == 32) ||
			(cluster == FREE_CLUSTER_16 && fat_type == 16) ||
			(cluster == FREE_CLUSTER_12 && fat_type == 12))
			return TRUE;

		return FALSE;
	}

	int FAT_set_cluster_free(unsigned int cluster, int fatType) {
		if (fat_type == 32) return FAT_write(cluster, FREE_CLUSTER_32);
		if (fat_type == 16) return FAT_write(cluster, FREE_CLUSTER_16);
		if (fat_type == 12) return FAT_write(cluster, FREE_CLUSTER_12);
	}

	BOOL FAT_cluster_end(unsigned int cluster, int fatType) {
		if ((cluster == END_CLUSTER_32 && fat_type == 32) ||
			(cluster == END_CLUSTER_16 && fat_type == 16) ||
			(cluster == END_CLUSTER_12 && fat_type == 12))
			return TRUE;

		return FALSE;
	}

	int FAT_set_cluster_end(unsigned int cluster, int fatType) {
		if (fat_type == 32) return FAT_write(cluster, END_CLUSTER_32);
		if (fat_type == 16) return FAT_write(cluster, END_CLUSTER_16);
		if (fat_type == 12) return FAT_write(cluster, END_CLUSTER_12);
	}

	BOOL FAT_cluster_bad(unsigned int cluster, int fatType) {
		if ((cluster == BAD_CLUSTER_32 && fat_type == 32) ||
			(cluster == BAD_CLUSTER_16 && fat_type == 16) ||
			(cluster == BAD_CLUSTER_12 && fat_type == 12))
			return TRUE;

		return FALSE;
	}

	int FAT_set_cluster_bad(unsigned int cluster, int fatType) {
		if (fat_type == 32) return FAT_write(cluster, BAD_CLUSTER_32);
		if (fat_type == 16) return FAT_write(cluster, BAD_CLUSTER_16);
		if (fat_type == 12) return FAT_write(cluster, BAD_CLUSTER_12);
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____  _____    _    ____    _____ _  _____ 
//  |  _ \| ____|  / \  |  _ \  |  ___/ \|_   _|
//  | |_) |  _|   / _ \ | | | | | |_ / _ \ | |  
//  |  _ <| |___ / ___ \| |_| | |  _/ ___ \| |  
//  |_| \_\_____/_/   \_\____/  |_|/_/   \_\_|  
//
// This function reads FAT table for getting cluster status (or cluster chain)

	int FAT_read(unsigned int clusterNum) {
		if (clusterNum < 2 || clusterNum >= total_clusters) {
			printf("Function FAT_read: invalid cluster number! [%i]\n", clusterNum);
			return -1;
		}

		if (fat_type == 32 || fat_type == 16) {
			unsigned int cluster_size 	= bytes_per_sector * sectors_per_cluster;
			unsigned int fat_offset 	= clusterNum * (fat_type == 16 ? 2 : 4);
			unsigned int fat_sector 	= first_fat_sector + (fat_offset / cluster_size);
			unsigned int ent_offset 	= fat_offset % cluster_size;

			//at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
			
			uint8_t* cluster_data = ATA_read_sectors(fat_sector, sectors_per_cluster);
			if (cluster_data == NULL) {
				printf("Function FAT_read: Could not read sector that contains FAT32 table entry needed.\n");
				return -1;
			}

			unsigned int table_value = *(unsigned int*)&cluster_data[ent_offset];
			if (fat_type == 32) table_value &= 0x0FFFFFFF;

			//the variable "table_value" now has the information you need about the next cluster in the chain.
			free(cluster_data);
			return table_value;
		}
		
		else {
			printf("Function FAT_read: Invalid fat_type value. The value was: %i\n", fat_type);
			return -1;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//  __        ______  ___ _____ _____   _____ _  _____ 
//  \ \      / /  _ \|_ _|_   _| ____| |  ___/ \|_   _|
//   \ \ /\ / /| |_) || |  | | |  _|   | |_ / _ \ | |  
//    \ V  V / |  _ < | |  | | | |___  |  _/ ___ \| |  
//     \_/\_/  |_| \_\___| |_| |_____| |_|/_/   \_\_|  
//
// This function writes cluster status to FAT table

	int FAT_write(unsigned int clusterNum, unsigned int clusterVal) {
		if (clusterNum < 2 || clusterNum >= total_clusters) {
			printf("Function FAT_write: invalid cluster number!\n");
			return -1;
		}

		if (fat_type == 32 || fat_type == 16) {
			unsigned int cluster_size 	= bytes_per_sector * sectors_per_cluster;
			unsigned int fat_offset 	= clusterNum * (fat_type == 16 ? 2 : 4);
			unsigned int fat_sector 	= first_fat_sector + (fat_offset / cluster_size);
			unsigned int ent_offset 	= fat_offset % cluster_size;

			uint8_t* sector_data = ATA_read_sectors(fat_sector, sectors_per_cluster);
			if (sector_data == NULL) {
				printf("Function FAT_write: Could not read sector that contains FAT32 table entry needed.\n");
				return -1;
			}

			*(unsigned int*)&sector_data[ent_offset] = clusterVal;
			if (ATA_write_sectors(fat_sector, sector_data, sectors_per_cluster) != 1) {
				printf("Function FAT_write: Could not write new FAT32 cluster number to sector.\n");
				return -1;
			}

			free(sector_data);
			return 0;
		}

		else {
			printf("Function FAT_write: Invalid fat_type value. The value was: %i\n", fat_type);
			return -1;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//      _    _     _     ___   ____    _  _____ _____ 
//     / \  | |   | |   / _ \ / ___|  / \|_   _| ____|
//    / _ \ | |   | |  | | | | |     / _ \ | | |  _|  
//   / ___ \| |___| |__| |_| | |___ / ___ \| | | |___ 
//  /_/   \_\_____|_____\___/ \____/_/   \_\_| |_____|
//
// This function allocates free FAT cluster (FREE not mean empty. Allocated cluster free in FAT table)

	unsigned int lastAllocatedCluster = SECTOR_OFFSET;

	unsigned int FAT_cluster_allocate() {
		unsigned int free_cluster 	= BAD_CLUSTER_12;
		unsigned int bad_cluster 	= BAD_CLUSTER_12;
		unsigned int end_cluster 	= BAD_CLUSTER_12;

		if (fat_type == 32) {
			free_cluster    = FREE_CLUSTER_32;
			bad_cluster     = BAD_CLUSTER_32;
			end_cluster     = END_CLUSTER_32;
		}
		else if (fat_type == 16) {
			free_cluster    = FREE_CLUSTER_16;
			bad_cluster     = BAD_CLUSTER_16;
			end_cluster     = END_CLUSTER_16;
		}
		else if (fat_type == 12) {
			free_cluster    = FREE_CLUSTER_12;
			bad_cluster     = BAD_CLUSTER_12;
			end_cluster     = END_CLUSTER_12;
		}
		else {
			printf("Function FAT_cluster_allocate: fat_type is not valid!\n");
			return BAD_CLUSTER_12;
		}

		unsigned int cluster = lastAllocatedCluster;
		unsigned int clusterStatus = free_cluster;

		while (cluster < total_clusters) {
			clusterStatus = FAT_read(cluster);
			if (clusterStatus == free_cluster) {
				if (FAT_write(cluster, end_cluster) == 0) {
					lastAllocatedCluster = cluster;
					return cluster;
				}
				else {
					printf("Function FAT_cluster_allocate: Error occurred with FAT_write, aborting operations...\n");
					return bad_cluster;
				}
			}
			else if (clusterStatus < 0) {
				printf("Function FAT_cluster_allocate: Error occurred with FAT_read, aborting operations...\n");
				return bad_cluster;
			}

			cluster++;
		}

		lastAllocatedCluster = 2;
		return bad_cluster;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____  _____    _    _     _     ___   ____    _  _____ _____ 
//  |  _ \| ____|  / \  | |   | |   / _ \ / ___|  / \|_   _| ____|
//  | | | |  _|   / _ \ | |   | |  | | | | |     / _ \ | | |  _|  
//  | |_| | |___ / ___ \| |___| |__| |_| | |___ / ___ \| | | |___ 
//  |____/|_____/_/   \_\_____|_____\___/ \____/_/   \_\_| |_____|
//
// This function deallocates clusters. Just mark them free in FAT table

	int FAT_cluster_deallocate(const unsigned int cluster) {
		if (fat_type != 12 && fat_type != 16 && fat_type != 32) {
			printf("Function FAT_cluster_allocate: fat_type is not valid!\n");
			return BAD_CLUSTER_12;
		}

		unsigned int clusterStatus = FAT_read(cluster);
		if (FAT_cluster_free(clusterStatus, fat_type) == TRUE) return 0;
		else if (clusterStatus < 0) {
			printf("Function FAT_cluster_deallocate: Error occurred with FAT_read, aborting operations...\n");
			return -1;
		}

		if (FAT_set_cluster_free(cluster, fat_type) == 0) return 0;
		else {
			printf("Function FAT_cluster_deallocate: Error occurred with FAT_write, aborting operations...\n");
			return -1;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ _    _   _ ____ _____ _____ ____    ____  _____    _    ____  
//   / ___| |  | | | / ___|_   _| ____|  _ \  |  _ \| ____|  / \  |  _ \ 
//  | |   | |  | | | \___ \ | | |  _| | |_) | | |_) |  _|   / _ \ | | | |
//  | |___| |__| |_| |___) || | | |___|  _ <  |  _ <| |___ / ___ \| |_| |
//   \____|_____\___/|____/ |_| |_____|_| \_\ |_| \_\_____/_/   \_\____/ 
//
// Reads one cluster
// This function deals in absolute data clusters

	uint8_t* FAT_cluster_read(unsigned int clusterNum) {
		if (clusterNum < 2 || clusterNum >= total_clusters) {
			printf("Function FAT_cluster_read: Invalid cluster number! [%u]\n", clusterNum);
			return NULL;
		}

		unsigned int start_sect = (clusterNum - 2) * (unsigned short)sectors_per_cluster + first_data_sector;
		uint8_t* cluster_data = ATA_read_sectors(start_sect, sectors_per_cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_cluster_read: An error occurred with ATA_read_sector [%u]\n", start_sect);
			return NULL;
		} else return cluster_data;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ _    _   _ ____ _____ _____ ____   __        ______  ___ _____ _____ 
//   / ___| |  | | | / ___|_   _| ____|  _ \  \ \      / /  _ \|_ _|_   _| ____|
//  | |   | |  | | | \___ \ | | |  _| | |_) |  \ \ /\ / /| |_) || |  | | |  _|  
//  | |___| |__| |_| |___) || | | |___|  _ <    \ V  V / |  _ < | |  | | | |___ 
//   \____|_____\___/|____/ |_| |_____|_| \_\    \_/\_/  |_| \_\___| |_| |_____|
//
// Deals in absolute clusters
// contentsToWrite: contains a pointer to the data to be written to disk
// clusterNum: Specifies the on-disk cluster to write the data to

	int FAT_cluster_write(void* contentsToWrite, unsigned int clusterNum) {
		if (clusterNum < 2 || clusterNum >= total_clusters) {
			printf("Function FAT_cluster_write: Invalid cluster number!\n");
			return -1;
		}

		unsigned int start_sect = (clusterNum - 2) * (unsigned short)sectors_per_cluster + first_data_sector;
		if (ATA_write_sectors(start_sect, contentsToWrite, sectors_per_cluster) == -1) {
			printf("Function FAT_cluster_write: An error occurred with ATA_write_sector, the area in sector ");
			return -1;
		} else return 0;
	}

	int FAT_cluster_clear(unsigned int clusterNum) {
		if (clusterNum < 2 || clusterNum >= total_clusters) {
			printf("Function FAT_cluster_clear: Invalid cluster number!\n");
			return -1;
		}

		char clear[sectors_per_cluster];
		memset(clear, 0, sectors_per_cluster);

		unsigned int start_sect = (clusterNum - 2) * (unsigned short)sectors_per_cluster + first_data_sector;
		if (ATA_write_sectors(start_sect, clear, sectors_per_cluster) == -1) {
			printf("Function FAT_cluster_clear: An error occurred with ATA_write_sector, the area in sector ");
			return -1;
		} else return 0;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____ ___ ____  _____ ____ _____ ___  ______   __  _     ____  
//  |  _ \_ _|  _ \| ____/ ___|_   _/ _ \|  _ \ \ / / | |   / ___| 
//  | | | | || |_) |  _|| |     | || | | | |_) \ V /  | |   \___ \ 
//  | |_| | ||  _ <| |__| |___  | || |_| |  _ < | |   | |___ ___) |
//  |____/___|_| \_\_____\____| |_| \___/|_| \_\|_|   |_____|____/
//
// receives the cluster to list, and will list all regular entries and directories, plus whatever attributes are passed in
// returns: -1 is a general error

	struct FATDirectory* FAT_directory_list(const unsigned int cluster, unsigned char attributesToAdd, BOOL exclusive) {
		struct FATDirectory* currentDirectory = malloc(sizeof(struct FATDirectory));
		if (cluster < 2 || cluster >= total_clusters) {
			printf("Function FAT_directory_list: Invalid cluster number! [%u]\n", cluster);
			return NULL;
		}

		const unsigned char default_hidden_attributes = (FILE_HIDDEN | FILE_SYSTEM); //FILE_LONG_NAME is ALWAYS hidden.
		unsigned char attributes_to_hide = default_hidden_attributes;

		if (exclusive == FALSE) attributes_to_hide &= (~attributesToAdd);
		else if (exclusive == TRUE) attributes_to_hide = (~attributesToAdd);

		char* cluster_data = FAT_cluster_read(cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_directory_list: FAT_cluster_read encountered an error. Aborting...\n");
			return NULL;
		}

		directory_entry_t* file_metadata = (directory_entry_t*)cluster_data;
		unsigned int meta_pointer_iterator_count = 0;
		while (1) {
			if (file_metadata->file_name[0] == ENTRY_END) break;

			else if (strncmp(file_metadata->file_name, "..", 2) == 0 || strncmp(file_metadata->file_name, ".", 1) == 0) {
				file_metadata++;
				meta_pointer_iterator_count++;
			}

			else if (((file_metadata->file_name)[0] == ENTRY_FREE)) {	
				if (meta_pointer_iterator_count < bytes_per_sector * sectors_per_cluster / sizeof(directory_entry_t) - 1) {
					file_metadata++;
					meta_pointer_iterator_count++;
				}
				else {
					unsigned int next_cluster = FAT_read(cluster);
					if (FAT_cluster_end(next_cluster, fat_type) == TRUE) break;
					else if (next_cluster < 0) {
						printf("Function FAT_directory_list: FAT_read encountered an error. Aborting...\n");
						return NULL;
					}
					else return FAT_directory_list(next_cluster, attributesToAdd, exclusive);
				}
			}

			else {
				if ((file_metadata->attributes & FILE_DIRECTORY) != FILE_DIRECTORY) {
					struct FATFile* file = malloc(sizeof(struct FATFile));
					file->file_meta = *file_metadata;

					if ((file_metadata->attributes & FILE_LONG_NAME) != FILE_LONG_NAME) {
						char* name = malloc(strlen(file_metadata->file_name));
						strcpy(name, file_metadata->file_name);

						strncpy(file->name, strtok(name, " "), 11);
						strncpy(file->extension, strtok(NULL, " "), 4);
					}

					if (currentDirectory->files == NULL) currentDirectory->files = file;
					else {
						struct FATFile* current = currentDirectory->files;
						while (current->next != NULL) 
							current = current->next;
			
						current->next = file;
					}
				}
				else {
					if ((file_metadata->attributes & FILE_DIRECTORY) == FILE_DIRECTORY) {
						struct FATDirectory* upperDir = malloc(sizeof(struct FATDirectory));
						upperDir->directory_meta = *file_metadata;

						if ((file_metadata->attributes & FILE_LONG_NAME) != FILE_LONG_NAME) {
							char* name = malloc(strlen(file_metadata->file_name));
							strcpy(name, file_metadata->file_name);
							strncpy(upperDir->name, strtok(name, " "), 11);
						}

						if (currentDirectory->subDirectory == NULL) currentDirectory->subDirectory = upperDir;
						else {
							struct FATDirectory* current = currentDirectory->subDirectory;
							while (current->next != NULL) 
								current = current->next;
				
							current->next = upperDir;
						}
					}
				}

				file_metadata++;
				meta_pointer_iterator_count++;
			}
		}

		return currentDirectory;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____ ___ ____  _____ ____ _____ ___  ______   __  ____  _____    _    ____   ____ _   _ 
//  |  _ \_ _|  _ \| ____/ ___|_   _/ _ \|  _ \ \ / / / ___|| ____|  / \  |  _ \ / ___| | | |
//  | | | | || |_) |  _|| |     | || | | | |_) \ V /  \___ \|  _|   / _ \ | |_) | |   | |_| |
//  | |_| | ||  _ <| |__| |___  | || |_| |  _ < | |    ___) | |___ / ___ \|  _ <| |___|  _  |
//  |____/___|_| \_\_____\____| |_| \___/|_| \_\|_|   |____/|_____/_/   \_\_| \_\\____|_| |_|
//
// receives the cluster to read for a directory and the requested file, and will iterate through the directory's clusters - 
// returning the entry for the searched file/subfolder, or no file/subfolder
// return value holds success or failure code, file holds directory entry if file is found
// entryOffset points to where the directory entry was found in sizeof(directory_entry_t) starting from zero (can be NULL)
// returns: -1 is a general error, -2 is a "not found" error

	int FAT_directory_search(const char* filepart, const unsigned int cluster, directory_entry_t* file, unsigned int* entryOffset) {
		if (cluster < 2 || cluster >= total_clusters) {
			printf("Function FAT_directory_search: Invalid cluster number!\n");
			return -1;
		}

		char searchName[13] = { '\0' };
		strcpy(searchName, filepart);

		if (FAT_name_check(searchName) != 0)
			FAT_name2fatname(searchName);

		char* cluster_data = FAT_cluster_read(cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_directory_search: FAT_cluster_read encountered an error. Aborting...\n");
			return -1;
		}

		directory_entry_t* file_metadata = (directory_entry_t*)cluster_data;
		unsigned int meta_pointer_iterator_count = 0;
		while (1) {
			if (file_metadata->file_name[0] == ENTRY_END) break;

			else if (strncmp((char*)file_metadata->file_name, searchName, 11) != 0) {
				if (meta_pointer_iterator_count < bytes_per_sector * sectors_per_cluster / sizeof(directory_entry_t) - 1) {
					file_metadata++;
					meta_pointer_iterator_count++;
				}
				else {
					int next_cluster = FAT_read(cluster);
					if (FAT_cluster_end(next_cluster, fat_type) == TRUE) break;

					else if (next_cluster < 0) {
						printf("Function FAT_directory_search: FAT_read encountered an error. Aborting...\n");
						return -1;
					} else return FAT_directory_search(filepart, next_cluster, file, entryOffset);
				}
			}

			else {
				if (file != NULL) memcpy(file, file_metadata, sizeof(directory_entry_t));
				if (entryOffset != NULL) *entryOffset = meta_pointer_iterator_count;

				return 0;
			}
		}

		return -2;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____ ___ ____  _____ ____ _____ ___  ______   __     _    ____  ____  
//  |  _ \_ _|  _ \| ____/ ___|_   _/ _ \|  _ \ \ / /    / \  |  _ \|  _ \ 
//  | | | | || |_) |  _|| |     | || | | | |_) \ V /    / _ \ | | | | | | |
//  | |_| | ||  _ <| |__| |___  | || |_| |  _ < | |    / ___ \| |_| | |_| |
//  |____/___|_| \_\_____\____| |_| \___/|_| \_\|_|   /_/   \_\____/|____/ 
//                                                                   
// pass in the cluster to write the directory to and the directory struct to write.
// struct should only have a file name, attributes, and size. the rest will be filled in automatically

	int FAT_directory_add(const unsigned int cluster, directory_entry_t* file_to_add) {
		if (FAT_name_check(file_to_add->file_name) != 0) {
			printf("Function FAT_directory_add: file name supplied is invalid!");
			return -1;
		}

		char* cluster_data = FAT_cluster_read(cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_directory_add: FAT_cluster_read encountered an error. Aborting...\n");
			return -1;
		}

		directory_entry_t* file_metadata = (directory_entry_t*)cluster_data;
		unsigned int meta_pointer_iterator_count = 0;
		while (1) {
			if (file_metadata->file_name[0] != ENTRY_FREE && file_metadata->file_name[0] != ENTRY_END) {
				if (meta_pointer_iterator_count < bytes_per_sector * sectors_per_cluster / sizeof(directory_entry_t) - 1) {
					file_metadata++;
					meta_pointer_iterator_count++;
				}
				else {
					unsigned int next_cluster = FAT_read(cluster);
					if (FAT_cluster_end(next_cluster, fat_type) == TRUE) {
						next_cluster = FAT_cluster_allocate();
						if (FAT_cluster_bad(next_cluster, fat_type) == TRUE) {
							printf("Function FAT_directory_add: allocation of new cluster failed. Aborting...\n");
							free(cluster_data);
							return -1;
						}

						if (FAT_write(cluster, next_cluster) != 0) {
							printf("Function FAT_directory_add: extension of the cluster chain with new cluster failed. Aborting...\n");
							free(cluster_data);
							return -1;
						}
					}

					free(cluster_data);
					return FAT_directory_add(next_cluster, file_to_add);
				}
			}
			else {
				unsigned short dot_checker = 0;
				for (dot_checker = 0; dot_checker < 11; dot_checker++) 
					if (file_to_add->file_name[dot_checker] == '.') {
						printf("Function FAT_directory_add: Invalid file name!");
						free(cluster_data);
						return -1;
					}
				
				file_to_add->creation_date 			= FAT_current_date();
				file_to_add->creation_time 			= FAT_current_time();
				file_to_add->creation_time_tenths 	= FAT_current_time_temths();
				file_to_add->last_accessed 			= file_to_add->creation_date;
				file_to_add->last_modification_date = file_to_add->creation_date;
				file_to_add->last_modification_time = file_to_add->creation_time;

				unsigned int new_cluster = FAT_cluster_allocate();
				if (FAT_cluster_bad(new_cluster, fat_type) == TRUE) {
					printf("Function FAT_directory_add: allocation of new cluster failed. Aborting...\n");
					free(cluster_data);

					return -1;
				}
				
				file_to_add->low_bits 	= GET_ENTRY_LOW_BITS(new_cluster);
				file_to_add->high_bits 	= GET_ENTRY_HIGH_BITS(new_cluster);

				memcpy(file_metadata, file_to_add, sizeof(directory_entry_t));
				if (FAT_cluster_write(cluster_data, cluster) != 0) {
					printf("Function FAT_directory_add: Writing new directory entry failed. Aborting...\n");
					free(cluster_data);
					return -1;
				}

				free(cluster_data);
				return 0;
			}
		}

		free(cluster_data);
		return -1; //return error.
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____ ___ ____  _____ ____ _____ ___  ______   __  _____ ____ ___ _____ 
//  |  _ \_ _|  _ \| ____/ ___|_   _/ _ \|  _ \ \ / / | ____|  _ \_ _|_   _|
//  | | | | || |_) |  _|| |     | || | | | |_) \ V /  |  _| | | | | |  | |  
//  | |_| | ||  _ <| |__| |___  | || |_| |  _ < | |   | |___| |_| | |  | |  
//  |____/___|_| \_\_____\____| |_| \___/|_| \_\|_|   |_____|____/___| |_| 
//
// This function edit names of directory entries in cluster

	int FAT_directory_edit(const unsigned int cluster, directory_entry_t* oldMeta, directory_entry_t* newMeta) {
		if (FAT_name_check(oldMeta->file_name) != 0) {
			printf("Function FAT_directory_edit: Invalid file name!");
			return -1;
		}

		char* cluster_data = FAT_cluster_read(cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_directory_edit: FAT_cluster_read encountered an error. Aborting...\n");
			return -1;
		}

		directory_entry_t* file_metadata = (directory_entry_t*)cluster_data;
		unsigned int meta_pointer_iterator_count = 0;
		while (1) {
			if (strstr(file_metadata->file_name, oldMeta->file_name) == 0) {

				// Correct new_meta data
				oldMeta->last_accessed 		 	= FAT_current_date();
				oldMeta->last_modification_date = FAT_current_date();
				oldMeta->last_modification_time = FAT_current_time_temths();

				memset(oldMeta->file_name, ' ', 11);
				strncpy(oldMeta->file_name, newMeta->file_name, 11);

				memcpy(file_metadata, newMeta, sizeof(directory_entry_t));
				if (FAT_cluster_write(cluster_data, cluster) != 0) {
					printf("Function FAT_directory_edit: Writing updated directory entry failed. Aborting...\n");
					free(cluster_data);
					return -1;
				}

				return 0;
			} 
			
			else if (meta_pointer_iterator_count < bytes_per_sector * sectors_per_cluster / sizeof(directory_entry_t) - 1)  {
				file_metadata++;
				meta_pointer_iterator_count++;
			} 
			
			else {
				unsigned int next_cluster = FAT_read(cluster);
				if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12)) {
					printf("Function FAT_directory_edit: End of cluster chain reached. File not found. Aborting...\n");
					free(cluster_data);
					return -2;
				}

				free(cluster_data);
				return FAT_directory_edit(next_cluster, oldMeta, newMeta);
			}
		}

		free(cluster_data);
		return -1;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//   ____ ___ ____  _____ ____ _____ ___  ______   __  ____  _____ __  __  _____     _______ 
//  |  _ \_ _|  _ \| ____/ ___|_   _/ _ \|  _ \ \ / / |  _ \| ____|  \/  |/ _ \ \   / / ____|
//  | | | | || |_) |  _|| |     | || | | | |_) \ V /  | |_) |  _| | |\/| | | | \ \ / /|  _|  
//  | |_| | ||  _ <| |__| |___  | || |_| |  _ < | |   |  _ <| |___| |  | | |_| |\ V / | |___ 
//  |____/___|_| \_\_____\____| |_| \___/|_| \_\|_|   |_| \_\_____|_|  |_|\___/  \_/  |_____|
//
// This function mark data in FAT table as free and deallocates all clusters

	int FAT_directory_remove(const unsigned int cluster, const char* fileName) {
		if (FAT_name_check(fileName) != 0) {
			printf("Function FAT_directory_remove: Invalid file name!");
			return -1;
		}

		char* cluster_data = FAT_cluster_read(cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_directory_remove: FAT_cluster_read encountered an error. Aborting...\n");
			return -1;
		}

		directory_entry_t* file_metadata = (directory_entry_t*)cluster_data;
		unsigned int meta_pointer_iterator_count = 0;
		while (1) {
			if (strstr(file_metadata->file_name, fileName) == 0) {
				file_metadata->file_name[0] = ENTRY_FREE;
				if (FAT_cluster_write(cluster_data, cluster) != 0) {
					printf("Function FAT_directory_remove: Writing updated directory entry failed. Aborting...\n");
					free(cluster_data);
					return -1;
				}

				return 0;
			} 
			
			else if (meta_pointer_iterator_count < bytes_per_sector * sectors_per_cluster / sizeof(directory_entry_t) - 1)  {
				file_metadata++;
				meta_pointer_iterator_count++;
			} 
			
			else {
				unsigned int next_cluster = FAT_read(cluster);
				if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12)) {
					printf("Function FAT_directory_remove: End of cluster chain reached. File not found. Aborting...\n");
					free(cluster_data);
					return -2;
				}

				free(cluster_data);
				return FAT_directory_remove(next_cluster, fileName);
			}
		}

		free(cluster_data);
		return -1; // Return error
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ ___  _   _ _____ _____ _   _ _____   _______  _____ ____ _____ 
//   / ___/ _ \| \ | |_   _| ____| \ | |_   _| | ____\ \/ /_ _/ ___|_   _|
//  | |  | | | |  \| | | | |  _| |  \| | | |   |  _|  \  / | |\___ \ | |  
//  | |__| |_| | |\  | | | | |___| |\  | | |   | |___ /  \ | | ___) || |  
//   \____\___/|_| \_| |_| |_____|_| \_| |_|   |_____/_/\_\___|____/ |_| 
//
// Function that checks is content exist
// returns: 0 if nexist and 1 if exist

	int FAT_content_exists(const char* filePath) {
		char fileNamePart[256];
		unsigned short start = 0;
		unsigned int active_cluster;

		if (fat_type == 32) active_cluster = ext_root_cluster;
		else {
			printf("Function FAT_content_exists: FAT16 and FAT12 are not supported!\n");
			return -1;
		}

		directory_entry_t file_info;
		for (unsigned int iterator = 0; iterator <= strlen(filePath); iterator++) {
			if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
				memset(fileNamePart, '\0', 256);
				memcpy(fileNamePart, filePath + start, iterator - start);

				int retVal = FAT_directory_search(fileNamePart, active_cluster, &file_info, NULL);
				if (retVal == -2) return 0;
				else if (retVal == -1) {
					printf("Function FAT_content_exists: An error occurred in FAT_directory_search. Aborting...\n");
					return -1;
				}

				start = iterator + 1;
				active_cluster = GET_CLUSTER_FROM_ENTRY(file_info);
			}
		}

		return 1; // Content exists
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ ___  _   _ _____ _____ _   _ _____    ____ _____ _____ 
//   / ___/ _ \| \ | |_   _| ____| \ | |_   _|  / ___| ____|_   _|
//  | |  | | | |  \| | | | |  _| |  \| | | |   | |  _|  _|   | |  
//  | |__| |_| | |\  | | | | |___| |\  | | |   | |_| | |___  | |  
//   \____\___/|_| \_| |_| |_____|_| \_| |_|    \____|_____| |_|  
//
// Returns: -1 is general error, -2 is directory not found, -3 is path specified is a directory instead of a file

	struct FATContent* FAT_get_content(const char* filePath) {
		struct FATContent* fatContent = malloc(sizeof(struct FATContent));
		char fileNamePart[256];
		unsigned short start = 0;
		unsigned int active_cluster;

		if (fat_type == 32) active_cluster = ext_root_cluster;
		else {
			printf("Function FAT_get_content: FAT16 and FAT12 are not supported!\n");
			free(fatContent);
			return -1;
		}

		directory_entry_t file_info;
		for (unsigned int iterator = 0; iterator <= strlen(filePath); iterator++) {
			if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
				memset(fileNamePart, '\0', 256);
				memcpy(fileNamePart, filePath + start, iterator - start);

				int retVal = FAT_directory_search(fileNamePart, active_cluster, &file_info, NULL);
				if (retVal == -2) {
					free(fatContent);
					return -2;
				}

				else if (retVal == -1) {
					printf("Function FAT_get_content: An error occurred in FAT_directory_search. Aborting...\n");
					free(fatContent);
					return retVal;
				}

				start = iterator + 1;
				active_cluster = GET_CLUSTER_FROM_ENTRY(file_info);
			}
		}

		fatContent->directory 					= malloc(sizeof(struct FATDirectory)); 
		fatContent->directory->directory_meta 	= file_info;

		char* name = malloc(strlen(file_info.file_name));
		strcpy(name, file_info.file_name);
		strncpy(fatContent->directory->name, strtok(name, " "), 11);

		fatContent->file = NULL;

		fatContent->directory->directory_meta = file_info;
		if ((file_info.attributes & FILE_DIRECTORY) != FILE_DIRECTORY) {
			if ((unsigned short)bytes_per_sector * (unsigned short)sectors_per_cluster + file_info.file_size > 262144) {
				printf("File too large.\n");
				free(fatContent);
				return -3;
			}

			fatContent->directory 	= NULL; // Mark that this content is noa a directory
			fatContent->file 		= malloc(sizeof(struct FATFile));

			int cluster = GET_CLUSTER_FROM_ENTRY(file_info);

			uint32_t* content = NULL;
			int content_size = 0;

			while (cluster < END_CLUSTER_32) {
				uint32_t* new_content = (uint32_t*)realloc(content, (content_size + 1) * sizeof(uint32_t));
				if (new_content == NULL) {
					free(content);
					return -1;
				}

				// Add the current cluster to the content array
				new_content[content_size] = cluster;

				content = new_content;
				content_size++;

				cluster = FAT_read(cluster);
				if (cluster == BAD_CLUSTER_32) {
					printf("Function FAT_get_content: the cluster chain is corrupted with a bad cluster. Aborting...\n");
					free(content);
					return -1;
				} else if (cluster == -1) {
					printf("Function FAT_get_content: an error occurred in FAT_read. Aborting...\n");
					free(content);
					return -1;
				}
			}
			
			fatContent->file->data = (char*)malloc((content_size) * sizeof(uint32_t));
			memcpy(fatContent->file->data, content, content_size * sizeof(uint32_t));
			fatContent->file->data_size = content_size;
			free(content);

			fatContent->file->file_meta = file_info;

			char* name = malloc(strlen(fatContent->file->file_meta.file_name));
			strcpy(name, fatContent->file->file_meta.file_name);

			strncpy(fatContent->file->name, strtok(name, " "), 11);
			strncpy(fatContent->file->extension, strtok(NULL, " "), 4);

			return fatContent; //file successfully found
		}
		else return fatContent;
	}

	char* FAT_read_content(struct FATContent* data) {
		int totalSize = sectors_per_cluster * SECTOR_SIZE * data->file->data_size;
		char* result = (char*)malloc(totalSize);
		memset(result, 0, totalSize);
		
		int offset = 0;
		for (int i = 0; i < data->file->data_size; i++) {
			uint8_t* content_part = FAT_cluster_read(data->file->data[i]);
			int size = SECTOR_SIZE * sectors_per_cluster;

			memcpy(result + offset, content_part, size);
			free(content_part);

			offset += size;
		}

		return result;
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ ___  _   _ _____ _____ _   _ _____   _____ ____ ___ _____ 
//   / ___/ _ \| \ | |_   _| ____| \ | |_   _| | ____|  _ \_ _|_   _|
//  | |  | | | |  \| | | | |  _| |  \| | | |   |  _| | | | | |  | |  
//  | |__| |_| | |\  | | | | |___| |\  | | |   | |___| |_| | |  | |  
//   \____\___/|_| \_| |_| |_____|_| \_| |_|   |_____|____/___| |_|  
//
// This function edit content in FAT content object
                                                                 
	void FAT_edit_content(const char* filePath, char* newData) {
		///////////////////////
		// FIND CONTENT

			struct FATContent* fatContent = FAT_get_content(filePath);
			if (fatContent == NULL) {
				printf("Function FAT_edit_content: FAT_get_content encountered an error. Aborting...\n");
				FAT_unload_content_system(fatContent);
				return -1;
			}

		// FIND CONTENT
		///////////////////////

		//////////////////////
		// CONTENT META SAVING

			directory_entry_t content_meta;
			if (fatContent->directory != NULL)
				content_meta = fatContent->directory->directory_meta;
			else if (fatContent->file != NULL)
				content_meta = fatContent->file->file_meta;

		// CONTENT META SAVING
		//////////////////////

		//////////////////////
		// EDIT DATA
			
			unsigned int cluster 			= GET_CLUSTER_FROM_ENTRY(content_meta);
			unsigned int dataLeftToWrite 	= strlen(newData);
			unsigned int allData			= dataLeftToWrite;

			while (cluster <= END_CLUSTER_32) {
				unsigned int dataWrite = 0;
				
				if (dataLeftToWrite >= bytes_per_sector * sectors_per_cluster) dataWrite = bytes_per_sector * sectors_per_cluster + 1;
				else dataWrite = dataLeftToWrite;

				char* sector_data = (char*)malloc(dataWrite + 1);
				memset(sector_data, 0, dataWrite + 1);
				strncpy(sector_data, newData, dataWrite);

				newData += dataWrite;

				if (FAT_cluster_bad(cluster, fat_type) == TRUE) {
					printf("Function FAT_edit_content: the cluster chain is corrupted with a bad cluster. Aborting...\n");
					return -1;
				}

				else if (cluster == -1 ) {
					printf("Function FAT_edit_content: an error occurred in FAT_read. Aborting...\n");
					return -1;
				}

				if (dataLeftToWrite > 0) {
					if (FAT_cluster_end(cluster, fat_type) == TRUE) {
						unsigned int newCluster = FAT_cluster_allocate();
						if (FAT_cluster_bad(newCluster, fat_type) == TRUE) {
							printf("Function FAT_edit_content: allocation of new cluster failed. Aborting...\n");
							return -1;
						}

						if (FAT_write(cluster, newCluster) != 0) {
							printf("Function FAT_edit_content: extension of the cluster chain with new cluster failed. Aborting...\n");
							return -1;
						}
					}
				} else if (dataLeftToWrite <= 0) {
					unsigned int prevCluster = cluster;
					unsigned int endCluster  = cluster;
					while (cluster < END_CLUSTER_32) {
						prevCluster = cluster;
						cluster = FAT_read(cluster);
						
						if (FAT_cluster_bad(cluster, fat_type) == TRUE) {
							printf("Function FAT_edit_content: allocation of new cluster failed. Aborting...\n");
							return -1;
						}

						if (FAT_cluster_deallocate(prevCluster) != 0) {
							printf("Deallocation problems.\n");
							break;
						}
					}
					
					if (FAT_cluster_end(endCluster, fat_type) != TRUE) 
						FAT_set_cluster_end(endCluster, fat_type);
					
					break;
				}

				char* previous_data = FAT_cluster_read(cluster);
				if (strstr(previous_data, sector_data) != 0) {
					FAT_cluster_clear(cluster);
					if (FAT_cluster_write(sector_data, cluster) != 0) {
						printf("Function FAT_edit_content: FAT_cluster_write encountered an error. Aborting...\n");
						free(previous_data);
						return -1;
					}

					free(previous_data);
				}

				dataLeftToWrite -= dataWrite;
				if (dataLeftToWrite == 0) {
					FAT_set_cluster_end(cluster, fat_type);

				}
				
				if (dataLeftToWrite < 0) {
					printf("Function FAT_edit_content: An undefined value has been detected. Aborting...\n");
					return -1;
				}
			}

			directory_entry_t* new_content = malloc(sizeof(directory_entry_t));
			memcpy(new_content, &content_meta, sizeof(directory_entry_t));

			new_content->last_accessed 		 	= FAT_current_date();
			new_content->last_modification_date = new_content->last_accessed;
			new_content->last_modification_time = FAT_current_time();
			new_content->file_size = allData;

			FAT_change_meta(filePath, new_content);

			free(new_content);
			return 0;
		
		// EDIT DATA
		//////////////////////

		return 0;
	}

//////////////////////////////////////////////////////////////////////////////////////////

// This function finds content in FAT table and change their name
int FAT_change_meta(const char* filePath, directory_entry_t* newMeta) {

	char fileNamePart[256];
	unsigned short start = 0;
	unsigned int active_cluster;
	unsigned int prev_active_cluster;

	//////////////////////
	//	FAT ACTIVE CLUSTER CHOOSING

		if (fat_type == 32) active_cluster = ext_root_cluster;
		else {
			printf("Function FAT_change_meta: FAT16 and FAT12 are not supported!\n");
			return -1;
		}

	//	FAT ACTIVE CLUSTER CHOOSING
	//////////////////////

	//////////////////////
	//	FINDING DIR BY PATH

		directory_entry_t file_info; //holds found directory info
		if (strlen(filePath) == 0) { // Create main dir if it not created (root dir)
			if (fat_type == 32) {
				active_cluster 			= ext_root_cluster;
				file_info.attributes 	= FILE_DIRECTORY | FILE_VOLUME_ID;
				file_info.file_size 	= 0;
				file_info.high_bits 	= GET_ENTRY_HIGH_BITS(active_cluster);
				file_info.low_bits 		= GET_ENTRY_LOW_BITS(active_cluster);
			}
			else {
				printf("Function FAT_change_meta: FAT16 and FAT12 are not supported!\n");
				return -1;
			}
		}
		else {
			for (unsigned int iterator = 0; iterator <= strlen(filePath); iterator++) 
				if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
					prev_active_cluster = active_cluster;

					memset(fileNamePart, '\0', 256);
					memcpy(fileNamePart, filePath + start, iterator - start);

					int retVal = FAT_directory_search(fileNamePart, active_cluster, &file_info, NULL);
					switch (retVal) {
						case -2:
							printf("Function FAT_change_meta: No matching directory found. Aborting...\n");
						return -2;

						case -1:
							printf("Function FAT_change_meta: An error occurred in FAT_directory_search. Aborting...\n");
						return retVal;
					}

					start = iterator + 1;
					active_cluster = GET_CLUSTER_FROM_ENTRY(file_info); //prep for next search
				}
		}

	//	FINDING DIR\FILE BY PATH
	//////////////////////

	//////////////////////
	// EDIT DATA

		if (FAT_directory_edit(prev_active_cluster, &file_info, newMeta) != 0) {
			printf("Function FAT_change_meta: FAT_directory_edit encountered an error. Aborting...\n");
			return -1;
		}
	
	// EDIT DATA
	//////////////////////

	return 0; // directory or file successfully deleted
}

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ ___  _   _ _____ _____ _   _ _____   ____  _   _ _____ 
//   / ___/ _ \| \ | |_   _| ____| \ | |_   _| |  _ \| | | |_   _|
//  | |  | | | |  \| | | | |  _| |  \| | | |   | |_) | | | | | |  
//  | |__| |_| | |\  | | | | |___| |\  | | |   |  __/| |_| | | |  
//   \____\___/|_| \_| |_| |_____|_| \_| |_|   |_|    \___/  |_| 
//
// writes a new file to the file system
// content: contains the full data of content (meta, name, ext, type)
// returns: -1 is general error, -2 indicates a bad path/file name, -3 indicates file with same name already exists, -4 indicates file size error

	int FAT_put_content(const char* filePath, struct FATContent* content) {

		//////////////////////
		// NAME ERROR HANDLING

			if (content->directory != NULL)
				if (FAT_name_check(content->directory->directory_meta.file_name) != 0) {
					printf("\nFunction FAT_put_content: Invalid directory name!\n");
					return -2;
				}
			else if (content->file != NULL)
				if (FAT_name_check(content->file->file_meta.file_name) != 0) {
					printf("\nFunction FAT_put_content: Invalid file name!\n");
					return -2;
				}

		// NAME ERROR HANDLING
		//////////////////////

		//////////////////////
		// CONTENT META SAVING

			directory_entry_t content_meta;
			if (content->directory != NULL)
				content_meta = content->directory->directory_meta;
			else if (content->file != NULL)
				content_meta = content->file->file_meta;

		// CONTENT META SAVING
		//////////////////////
			
		char fileNamePart[256];
		unsigned short start = 0;
		unsigned int active_cluster;

		//////////////////////
		//	FAT ACTIVE CLUSTER CHOOSING

			if (fat_type == 32) active_cluster = ext_root_cluster;
			else {
				printf("Function FAT_put_content: FAT16 and FAT12 are not supported!\n");
				return -1;
			}

		//	FAT ACTIVE CLUSTER CHOOSING
		//////////////////////

		//////////////////////
		//	FINDING DIR BY PATH

			directory_entry_t file_info; //holds found directory info
			if (strlen(filePath) == 0) { // Create main dir if it not created (root dir)
				if (fat_type == 32) {
					active_cluster 			= ext_root_cluster;
					file_info.attributes 	= FILE_DIRECTORY | FILE_VOLUME_ID;
					file_info.file_size 	= 0;
					file_info.high_bits 	= GET_ENTRY_HIGH_BITS(active_cluster);
					file_info.low_bits 		= GET_ENTRY_LOW_BITS(active_cluster);
				}
				else {
					printf("Function FAT_put_content: FAT16 and FAT12 are not supported!\n");
					return -1;
				}
			}
			else {
				for (unsigned int iterator = 0; iterator <= strlen(filePath); iterator++) 
					if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
						memset(fileNamePart, '\0', 256);
						memcpy(fileNamePart, filePath + start, iterator - start);

						int retVal = FAT_directory_search(fileNamePart, active_cluster, &file_info, NULL);
						switch (retVal) {
							case -2:
								printf("Function FAT_put_content: No matching directory found. Aborting...\n");
							return -2;

							case -1:
								printf("Function FAT_put_content: An error occurred in FAT_directory_search. Aborting...\n");
							return retVal;
						}

						start = iterator + 1;
						active_cluster = GET_CLUSTER_FROM_ENTRY(file_info); //prep for next search
					}
			}

		//	FINDING DIR\FILE BY PATH
		//////////////////////

		//////////////////////
		// CHECK IF FILE EXIST
		// A.i.: directory to receive the file is now found, and its cluster 
		// is stored in active_cluster. Search the directory to ensure the 
		// specified file name is not already in use
		
			char output[13];
			FAT_fatname2name((char*)content_meta.file_name, output);
			int retVal = FAT_directory_search(output, active_cluster, NULL, NULL);
			if (retVal == -1) {
				printf("Function putFile: directorySearch encountered an error. Aborting...\n");
				return -1;
			}
			else if (retVal != -2) {
				printf("Function putFile: a file matching the name given already exists. Aborting...\n");
				return -3;
			}

		// CHECK IF FILE EXIST
		//////////////////////

		if ((file_info.attributes & FILE_DIRECTORY) == FILE_DIRECTORY) {
			if (FAT_directory_add(active_cluster, &content_meta) != 0) {
				printf("Function FAT_put_content: FAT_directory_add encountered an error. Aborting...\n");
				return -1;
			}

			//////////////////////
			// CHECK IF FILE EXIST AFTER ADD
			// A.i.: now filling file_info with the information of the file directory entry

				char output[13];
				FAT_fatname2name((char*)content_meta.file_name, output);
				int retVal = FAT_directory_search(output, active_cluster, &file_info, NULL);
				if (retVal == -2) {
					printf("Function FAT_put_content: FAT_directory_add did not properly write the new file's entry to disk. Aborting...\n");
					return -2;
				}
				else if (retVal != 0) {
					printf("Function FAT_put_content: FAT_directory_search has encountered an error. Aborting...\n");
					return -1;
				}

			// CHECK IF FILE EXIST AFTER ADD
			//////////////////////

			active_cluster = GET_CLUSTER_FROM_ENTRY(file_info);
			if ((file_info.attributes & FILE_DIRECTORY) != FILE_DIRECTORY) {
				unsigned int dataLeftToWrite = content_meta.file_size;
				while (dataLeftToWrite > 0) {
					unsigned int dataWrite = 0;
					if (dataLeftToWrite >= bytes_per_sector * sectors_per_cluster) dataWrite = bytes_per_sector * sectors_per_cluster + 1;
					else dataWrite = dataLeftToWrite;

					char* data = FAT_read_content(content);
					if (FAT_cluster_write(data, active_cluster) != 0) {
						printf("Function FAT_put_content: FAT_cluster_write encountered an error. Aborting...\n");
						free(data);
						return -1;
					}

					free(data);

					dataLeftToWrite -= dataWrite;
					if (dataLeftToWrite == 0) break;
					else if (dataLeftToWrite < 0) {
						printf("Function FAT_put_content: An undefined value has been detected. Aborting...\n");
						return -1;
					}

					unsigned int new_cluster = FAT_cluster_allocate();
					if (FAT_cluster_bad(new_cluster, fat_type) == TRUE) {
						printf("Function FAT_put_content: FAT_cluster_allocate encountered an error. Aborting...\n");
						return -1;
					}

					if (FAT_write(active_cluster, new_cluster) != 0) {
						printf("Function FAT_put_content: FAT_write encountered an error. Aborting...\n");
						return -1;
					}

					active_cluster = new_cluster;
				}
			}

			return 0; //file successfully written
		}
		else {
			printf ("Function FAT_put_content: Invalid path!\n");
			return -2; //invalid path!
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ____ ___  _   _ _____ _____ _   _ _____   ____  _____ _     _____ _____ _____ 
//   / ___/ _ \| \ | |_   _| ____| \ | |_   _| |  _ \| ____| |   | ____|_   _| ____|
//  | |  | | | |  \| | | | |  _| |  \| | | |   | | | |  _| | |   |  _|   | | |  _|  
//  | |__| |_| | |\  | | | | |___| |\  | | |   | |_| | |___| |___| |___  | | | |___ 
//   \____\___/|_| \_| |_| |_____|_| \_| |_|   |____/|_____|_____|_____| |_| |_____|
//
// This function delete content from FS
// filePath - path where placed content
// name - name of content (if it file - with extension (if presented), like "test.txt")

	int FAT_delete_content(const char* filePath, const char* name) {

		///////////////////////
		// FIND CONTENT

			struct FATContent* fatContent = FAT_get_content(filePath);
			if (fatContent == NULL) {
				printf("Function FAT_delete_content: FAT_get_content encountered an error. Aborting...\n");
				return -1;
			}

			char contentName[13] = { '\0' };
			strcpy(contentName, name);

			if (FAT_name_check(contentName) != 0) 
				FAT_name2fatname(contentName);

		// FIND CONTENT
		///////////////////////

		//////////////////////
		// CONTENT META SAVING

			directory_entry_t content_meta;
			if (fatContent->directory != NULL)
				content_meta = fatContent->directory->directory_meta;
			else if (fatContent->file != NULL)
				content_meta = fatContent->file->file_meta;

		// CONTENT META SAVING
		//////////////////////

		//////////////////////
		// DELETE DATA

			unsigned int directory_cluster = GET_CLUSTER_FROM_ENTRY(content_meta);

			directory_entry_t file_meta;
			FAT_directory_search(name, directory_cluster, &file_meta, NULL);
			unsigned int data_cluster = GET_CLUSTER_FROM_ENTRY(file_meta);
			unsigned int prev_cluster = prev_cluster;
			
			while (data_cluster < END_CLUSTER_32) {
				prev_cluster = FAT_read(data_cluster);
				if (FAT_cluster_deallocate(data_cluster) != 0) {
					printf("Function FAT_delete_content: FAT_cluster_deallocate encountered an error. Aborting...\n");
					return -1;
				}

				data_cluster = prev_cluster;
			}

			if (FAT_directory_remove(directory_cluster, contentName) != 0) {
				printf("Function FAT_delete_content: FAT_directory_remove encountered an error. Aborting...\n");
				free(fatContent->directory);
				free(fatContent);
				return -1;
			}
		
		// DELETE DATA
		//////////////////////

		// Free allocated memory
		if (fatContent->directory != NULL) {
			free(fatContent->directory);
		} else {
			free(fatContent->file->data);
			free(fatContent->file);
		}
		free(fatContent);

		return 0; // directory or file successfully deleted
	}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//    ___ _____ _   _ _____ ____  
//   / _ \_   _| | | | ____|  _ \ 
//  | | | || | | |_| |  _| | |_) |
//  | |_| || | |  _  | |___|  _ < 
//   \___/ |_| |_| |_|_____|_| \_\
//
// Other functions that used here

	// clock hasn't been implemented yet
	unsigned short FAT_current_time() {
		datetime_read_rtc();
		return (datetime_hour << 11) | (datetime_minute << 5) | (datetime_second / 2);
	}

	//clock nor date has been implemented yet
	unsigned short FAT_current_date() {
		datetime_read_rtc();

		uint16_t reversed_data = 0;

		reversed_data |= datetime_day & 0x1F;
		reversed_data |= (datetime_month & 0xF) << 5;
		reversed_data |= ((datetime_year - 1980) & 0x7F) << 9;

		return reversed_data;
	}

	//clock hasn't been implemented yet
	unsigned char FAT_current_time_temths() {
		datetime_read_rtc();
		return (datetime_hour << 11) | (datetime_minute << 5) | (datetime_second / 2);
	}

	// date - 1 | time - 2
	struct FATDate* FAT_get_date(short data, int type) {
		struct FATDate* date = malloc(sizeof(struct FATDate));
		switch (type) {
			case 1: // date
				date->year   = ((data >> 9) & 0x7F) + 1980;
				date->mounth = (data >> 5) & 0xF;
				date->day    = data & 0x1F;
				return date;

			break;

			case 2: // time
				date->hour   = (data >> 11) & 0x1F;
				date->minute = (data >> 5) & 0x3F;
				date->second = (data & 0x1F) * 2;
				return date;

			break;
		}

		free(date);
		return NULL;
	}

	/*-----------------------------------------------------------------------------
	FAT_check_sum()
	Returns an unsigned byte checksum computed on an unsigned byte
	array. The array must be 11 bytes long and is assumed to contain
	a name stored in the format of a MS-DOS directory entry.
	Passed: pFcbName Pointer to an unsigned byte array assumed to be
	11 bytes long.
	Returns: Sum An 8-bit unsigned checksum of the array pointed
	to by pFcbName.
	------------------------------------------------------------------------------*/
	unsigned char FAT_check_sum(unsigned char *pFcbName) {
		short FcbNameLen;
		unsigned char Sum;
		Sum = 0;
		for (FcbNameLen = 11; FcbNameLen != 0; FcbNameLen--) 
			Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *pFcbName++;

		return (Sum);
	}

	char* FAT_name2fatname(char* input) {
		str_uppercase(input);

		BOOL haveExt = FALSE;
		char searchName[13] = { '\0' };
		unsigned short dotPos = 0;

		unsigned int counter = 0;
		while (counter <= 8) {
			if (input[counter] == '.' || input[counter] == '\0') {
				if (input[counter] == '.') haveExt = TRUE;

				dotPos = counter;
				counter++;

				break;
			}
			else {
				searchName[counter] = input[counter];
				counter++;
			}
		}

		if (counter > 9) {
			counter = 8;
			dotPos = 8;
		}
		
		unsigned short extCount = 8;
		while (extCount < 11) {
			if (input[counter] != '\0' && haveExt == TRUE) searchName[extCount] = input[counter];
			else searchName[extCount] = ' ';

			counter++;
			extCount++;
		}

		counter = dotPos;
		while (counter < 8) {
			searchName[counter] = ' ';
			counter++;
		}

		strcpy(input, searchName);
		return input;
	}

	//Return Codes:
	// -1: bad characters
	// -2: Lowercase
	BOOL FAT_name_check(char * input) {
		short retVal = 0;

		//Invalid Values:
		/*Values less than 0x20 except for the special case of 0x05 in DIR_Name[0] described above.
			 0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D,
			and 0x7C.*/
		unsigned short iterator;
		for (iterator = 0; iterator < 11; iterator++) {
			if (input[iterator] < 0x20 && input[iterator] != 0x05) {
				retVal = retVal | BAD_CHARACTER;
			}
			
			switch (input[iterator]) {
				case 0x2E: {
					if ((retVal & NOT_CONVERTED_YET) == NOT_CONVERTED_YET) //a previous dot has already triggered this case
						retVal |= TOO_MANY_DOTS;

					retVal ^= NOT_CONVERTED_YET; //remove NOT_CONVERTED_YET flag if already set

					break;
				}

				case 0x22:
				case 0x2A:
				case 0x2B:
				case 0x2C:
				case 0x2F:
				case 0x3A:
				case 0x3B:
				case 0x3C:
				case 0x3D:
				case 0x3E:
				case 0x3F:
				case 0x5B:
				case 0x5C:
				case 0x5D:
				case 0x7C:
					retVal = retVal | BAD_CHARACTER;
			}

			if (input[iterator] >= 'a' && input[iterator] <= 'z') 
				retVal = retVal | LOWERCASE_ISSUE;
		}

		return retVal;
	}

	//Converts the file name stored in a FAT directory entry into a more human-sensible format
	void FAT_fatname2name(char* input, char* output) {
		if (input[0] == '.') {
			if (input[1] == '.') {
				strcpy (output, "..");
				return;
			}

			strcpy (output, ".");
			return;
		}

		unsigned short counter = 0;
		for ( counter = 0; counter < 8; counter++) {
			if (input[counter] == 0x20) {
				output[counter] = '.';
				break;
			}

			output[counter] = input[counter];
		}

		if (counter == 8) 
			output[counter] = '.';

		unsigned short counter2 = 8;
		for (counter2 = 8; counter2 < 11; counter2++) {
			++counter;
			if (input[counter2] == 0x20 || input[counter2] == 0x20) {
				if (counter2 == 8)
					counter -= 2;

				break;
			}
			output[counter] = input[counter2];		
		}

		++counter;
		while (counter < 12) {
			output[counter] = ' ';
			++counter;
		}

		output[12] = '\0'; //ensures proper termination regardless of program operation previously
		return;
	}

	// createEntry(&newDirEntry, dirName, extention, TRUE, beginNewDirClusterChain, 0, FALSE, FALSE);
	/* description: takes a directory entry and all the necesary info
		and populates the entry with the info in a correct format for
		insertion into a disk.
	*/
	struct directory_entry* FAT_create_entry(const char* filename, const char* ext, BOOL isDir, uint32_t firstCluster, uint32_t filesize) {
		struct directory_entry* data = malloc(sizeof(struct directory_entry));

		data->reserved0 				= 0; 
		data->creation_time_tenths 		= 0;
		data->creation_time 			= 0;
		data->creation_date 			= 0;
		data->last_modification_date 	= 0;

		char* file_name = (char*)malloc(25);
		strcpy(file_name, filename);
		if (ext) {
			strcat(file_name, ".");
			strcat(file_name, ext);
		}
		
		data->low_bits 	= firstCluster;
		data->high_bits = firstCluster >> 16;  

		if(isDir == TRUE) {
			data->file_size 	= 0;
			data->attributes 	= FILE_DIRECTORY;
		} else {
			data->file_size 	= filesize;
			data->attributes 	= FILE_ARCHIVE;
		}

		data->creation_date 		 = FAT_current_date();
		data->creation_time 		 = FAT_current_time();
		data->creation_time_tenths 	 = FAT_current_time_temths();

		if (FAT_name_check(file_name) != 0)
			FAT_name2fatname(file_name);

		strncpy(data->file_name, file_name, min(11, strlen(file_name)));
		return data; 
	}

	void FAT_unload_directories_system(struct FATDirectory* directory) {
		struct FATFile* current_file = directory->files;
		struct FATFile* next_file    = NULL;
		while (current_file != NULL) {
			next_file = current_file->next;
			free(current_file);
			current_file = next_file;
		}

		if (directory->subDirectory != NULL)
			FAT_unload_directories_system(directory->subDirectory);

		if (directory->next != NULL) 
			FAT_unload_directories_system(directory->next);

		free(directory);
	}

	void FAT_unload_files_system(struct FATFile* file) {
		struct FATFile* current_file = file;
		struct FATFile* next_file    = NULL;
		while (current_file != NULL) {
			next_file = current_file->next;
			free(current_file);
			current_file = next_file;
		}
	}

	struct FATContent* FAT_create_content(char* name, BOOL directory, char* extension) {
		struct FATContent* content = malloc(sizeof(struct FATContent));
		if (strlen(name) > 11 || strlen(extension) > 4) {
			printf("Uncorrect name or ext lenght.\n");
			return NULL;
		}
		
		if (directory == TRUE) {
			content->directory = malloc(sizeof(struct FATDirectory));
			strncpy(content->directory->name, name, 11);
			content->directory->directory_meta = *FAT_create_entry(name, NULL, TRUE, NULL, NULL);
		}

		else {
			content->file = malloc(sizeof(struct FATFile));
			strncpy(content->file->name, name, 11);
			strncpy(content->file->extension, extension, 4);
			content->file->file_meta = *FAT_create_entry(name, extension, FALSE, NULL, 1);
		}

		return content;
	}

	void FAT_unload_content_system(struct FATContent* content) {
		if (content->directory != NULL)
			FAT_unload_directories_system(content->directory);
		
		if (content->file != NULL)
			FAT_unload_files_system(content->file);
	}

	char* FAT_change_path(const char* currentPath, const char* content) {
		if (content == NULL || content[0] == '\0') {
			const char* lastSeparator = strrchr(currentPath, '\\');
			if (lastSeparator == NULL) currentPath = "";
			else {
				size_t parentPathLen = lastSeparator - currentPath;
				char* parentPath = (char*)malloc(parentPathLen + 1);
				if (parentPath == NULL) {
					printf("Memory allocation failed\n");
					return NULL;
				}

				strncpy(parentPath, currentPath, parentPathLen);
				parentPath[parentPathLen] = '\0';

				free((void*)currentPath);
				currentPath = parentPath;
			}
		} else {
			size_t newPathLen = strlen(currentPath) + strlen(content) + 2;
			char* newPath = (char*)malloc(newPathLen);
			if (newPath == NULL) {
				printf("Memory allocation failed\n");
				return NULL;
			}

			strcpy(newPath, currentPath);
			if (newPath[strlen(newPath) - 1] != '\\') 
				strcat(newPath, "\\");
			
			strcat(newPath, content);

			free((void*)currentPath);
			currentPath = newPath;
		}

		return strdup(currentPath);
	}

	char* FAT_get_current_path() {
		return current_path;
	}

	void FAT_set_current_path(char* path) {
		current_path = path;
	}

//////////////////////////////////////////////////////////////////////////////////////////