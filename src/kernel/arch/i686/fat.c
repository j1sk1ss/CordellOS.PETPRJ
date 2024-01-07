#include "../../include/fat.h"

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

//Initializes struct "bootsect" to store critical data from the boot sector of the volume
int FAT_initialize() {
    uint8_t* cluster_data = ATA_read_sector(0, 1);
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

// TODO: FIX
//read FAT table
//This function deals in absolute data clusters
int FAT_read(unsigned int clusterNum) {
	if (clusterNum < 2 || clusterNum >= total_clusters) {
		printf("Function FAT_read: invalid cluster number!\n");
		return -1;
	}

	if (fat_type == 32 || fat_type == 16) {
		unsigned int cluster_size 	= bytes_per_sector * sectors_per_cluster;
		unsigned int fat_offset 	= clusterNum * 4;
		unsigned int fat_sector 	= first_fat_sector + (fat_offset / cluster_size);
		unsigned int ent_offset 	= fat_offset % cluster_size;

		//at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
        char* cluster_data = ATA_read_sector(fat_sector, sectors_per_cluster);
		if (cluster_data == NULL) {
			printf("Function FAT_read: Could not read sector that contains FAT32 table entry needed.\n");
			return -1;
		}

		unsigned int table_value = cluster_data[ent_offset] & 0x0FFFFFFF;

		//the variable "table_value" now has the information you need about the next cluster in the chain.
		free(cluster_data);
		return table_value;
	}
    
	else {
		printf("Function FAT_read: Invalid fat_type value. The value was: %i\n", fat_type);
		return -1;
	}
}

// TODO: FIX
int FAT_write(unsigned int clusterNum, unsigned int clusterVal) {
	if (clusterNum < 2 || clusterNum >= total_clusters) {
		printf("Function FAT_write: invalid cluster number!\n");
		return -1;
	}

	if (fat_type == 32 || fat_type == 16) {
		unsigned int cluster_size 	= bytes_per_sector * sectors_per_cluster;
		unsigned int fat_offset 	= clusterNum * 4;
		unsigned int fat_sector 	= first_fat_sector + (fat_offset / cluster_size);
		unsigned int ent_offset 	= fat_offset % cluster_size;

        char* sector_data = ATA_read_sector(fat_sector, sectors_per_cluster);
		if (sector_data == NULL) {
			printf("Function FAT_write: Could not read sector that contains FAT32 table entry needed.\n");
			return -1;
		}

		sector_data[ent_offset] = clusterVal;
		if (ATA_write_sector(fat_sector, sectors_per_cluster, sector_data) != 1) {
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

unsigned int FAT_allocate_free() {
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
		printf("Function FAT_allocate_free: fat_type is not valid!\n");
		return BAD_CLUSTER_12;
	}

	unsigned int cluster = 2;
	unsigned int clusterStatus = free_cluster;

	while (cluster < total_clusters) {
		clusterStatus = FAT_read(cluster);
		if (clusterStatus == free_cluster) {
			if (FAT_write(cluster, end_cluster) == 0) return cluster;
			else {
				printf("Function FAT_allocate_free: Error occurred with FAT_write, aborting operations...\n");
				return bad_cluster;
			}
		}
		else if (clusterStatus < 0) {
			printf("Function FAT_allocate_free: Error occurred with FAT_read, aborting operations...\n");
			return bad_cluster;
		}

		cluster++;
	}

	return bad_cluster;
}

//Reads one cluster and dumps it to DISK_READ_LOCATION, offset "cluster_size" number of bytes from DISK_READ_LOCATION
//This function deals in absolute data clusters
char* FAT_cluster_read(unsigned int clusterNum, unsigned int clusterOffset) {
	if (clusterNum < 2 || clusterNum >= total_clusters) {
		printf("Function FAT_cluster_read: Invalid cluster number! [%u]\n", clusterNum);
		return NULL;
	}

	unsigned int start_sect = (clusterNum - 2) * (unsigned short)sectors_per_cluster + first_data_sector;
    char* cluster_data = ATA_read_sector(start_sect, sectors_per_cluster);
    if (cluster_data == NULL) {
        printf("Function FAT_cluster_read: An error occurred with ATA_read_sector [%u]\n", start_sect);
        return NULL;
    } else return cluster_data;
}

//Deals in absolute clusters
//contentsToWrite: contains a pointer to the data to be written to disk
//contentSize: contains how big contentsToWrite's data is (in bytes)
//contentBuffOffset: sets how far offset from DISK_WRITE_LOCATION to place the data from contentsToWrite in preparation for writing to disk (in clusters)
//clusterNum: Specifies the on-disk cluster to write the data to
int FAT_cluster_write(void* contentsToWrite, unsigned int contentSize, unsigned int contentBuffOffset, unsigned int clusterNum) {
	if (clusterNum < 2 || clusterNum >= total_clusters) {
		printf("Function FAT_cluster_write: Invalid cluster number!\n");
		return -1;
	}

	unsigned int start_sect = (clusterNum - 2) * (unsigned short)sectors_per_cluster + first_data_sector;
    if (ATA_write_sector(start_sect, sectors_per_cluster, contentsToWrite) == -1) {
        printf("Function FAT_cluster_write: An error occurred with ATA_write_sector, the area in sector ");
        return -1;
    } else return 0;
}

//. and .. entries not supported yet!

//receives the cluster to list, and will list all regular entries and directories, plus whatever attributes are passed in
//returns: -1 is a general error
int FAT_directory_list(const unsigned int cluster, unsigned char attributesToAdd, BOOL exclusive) {
	if (cluster < 2 || cluster >= total_clusters) {
		printf("Function FAT_directory_list: Invalid cluster number! [%u]\n", cluster);
		return -1;
	}

	const unsigned char default_hidden_attributes = (FILE_HIDDEN | FILE_SYSTEM); //FILE_LONG_NAME is ALWAYS hidden.
	unsigned char attributes_to_hide = default_hidden_attributes;

	if (exclusive == FALSE) attributes_to_hide &= (~attributesToAdd);
	else if (exclusive == TRUE) attributes_to_hide = (~attributesToAdd);

    char* cluster_data = FAT_cluster_read(cluster, 0);
	if (cluster_data == NULL) {
		printf("Function FAT_directory_list: FAT_cluster_read encountered an error. Aborting...\n");
		return -1;
	}

	directory_entry_t* file_metadata = (directory_entry_t*)cluster_data;
	unsigned int meta_pointer_iterator_count = 0; //holds how many directories have been looked at

	while (1) {
		if (file_metadata->file_name[0] == ENTRY_END) {
			break;
		}
		else if (strncmp(file_metadata->file_name, "..", 2) == 0 || strncmp(file_metadata->file_name, ".", 1) == 0) {
			if (file_metadata->file_name[1] == '.') printf("..");
			else printf(".");

			printf("\t\t\tDIR\n");

			file_metadata++;
			meta_pointer_iterator_count++;
		}
		else if (((file_metadata->file_name)[0] == ENTRY_FREE) || ((file_metadata->attributes & FILE_LONG_NAME) == FILE_LONG_NAME) || ((file_metadata->attributes & attributes_to_hide) != 0)) {	
			if (meta_pointer_iterator_count < bytes_per_sector * sectors_per_cluster / sizeof(directory_entry_t) - 1) {
				file_metadata++;
				meta_pointer_iterator_count++;
			}
			else //search next cluster in directory
			{
				unsigned int next_cluster = FAT_read(cluster);

				if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12))
					break;
				else if (next_cluster < 0)
				{
					printf("Function FAT_directory_list: FAT_read encountered an error. Aborting...\n");
					return -1;
				}
				else
					return FAT_directory_list(next_cluster, attributesToAdd, exclusive); //search next cluster
			}
		}
		else {
			char conversion [13];
			FAT_fatname2name((char*)file_metadata->file_name, conversion);
			printf(conversion);
			printf("\t");

			if ((file_metadata->attributes & FILE_DIRECTORY) != FILE_DIRECTORY) printf("%u", file_metadata->file_size);
			else printf("\t");

			printf("\t");
			if ((file_metadata->attributes & FILE_DIRECTORY) == FILE_DIRECTORY) printf("DIR");
			
			printf("\n");

			file_metadata++;
			meta_pointer_iterator_count++;
		}
	}

	return 0; //done searching
}

//receives the cluster to read for a directory and the requested file, and will iterate through the directory's clusters - returning the entry for the searched file/subfolder, or no file/subfolder
//return value holds success or failure code, file holds directory entry if file is found
//entryOffset points to where the directory entry was found in sizeof(directory_entry_t) starting from zero (can be NULL)
//returns: -1 is a general error, -2 is a "not found" error
int FAT_directory_search(const char* filepart, const unsigned int cluster, directory_entry_t* file, unsigned int* entryOffset) {
	if (cluster < 2 || cluster >= total_clusters) {
		printf("Function FAT_directory_search: Invalid cluster number!\n");
		return -1;
	}

	char searchName[13] = { '\0' };
	strcpy(searchName, filepart);

	//the file path piece sent in is not in FAT format; convert.
	if (FAT_name_check(searchName) != 0)
		FAT_name2fatname(searchName);

	//read cluster of the directory/subdirectory
    char* cluster_data = FAT_cluster_read(cluster, 0);
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

				if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12))
					break;
				else if (next_cluster < 0) {
					printf("Function FAT_directory_search: FAT_read encountered an error. Aborting...\n");
					return -1;
				}
				else return FAT_directory_search(filepart, next_cluster, file, entryOffset); //search next cluster
			}
		}
		else {
			if (file != NULL) memcpy(file, file_metadata, sizeof(directory_entry_t)); //copy found data to file
			if (entryOffset != NULL) *entryOffset = meta_pointer_iterator_count;

			return 0;
		}
	}

	return -2; //nothing found, return error.
}

//pass in the cluster to write the directory to and the directory struct to write.
//struct should only have a file name, attributes, and size. the rest will be filled in automatically
int FAT_directory_add(const unsigned int cluster, directory_entry_t* file_to_add) {
	if (FAT_name_check(file_to_add->file_name) != 0) {
		printf("Function FAT_directory_add: file name supplied is invalid!");
		return -1;
	}

    char* cluster_data = FAT_cluster_read(cluster, 0);
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
				if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12)) {
					next_cluster = FAT_allocate_free();
					if ((next_cluster == BAD_CLUSTER_32 && fat_type == 32) || (next_cluster == BAD_CLUSTER_16 && fat_type == 16) || (next_cluster == BAD_CLUSTER_12 && fat_type == 12)) {
						printf("Function FAT_directory_add: allocation of new cluster failed. Aborting...\n");
						free(file_metadata);
						return -1;
					}

					if (FAT_write(cluster, next_cluster) != 0) {
						printf("Function FAT_directory_add: extension of the cluster chain with new cluster failed. Aborting...\n");
						free(file_metadata);
						return -1;
					}
				}

				free(file_metadata);
				return FAT_directory_add(next_cluster, file_to_add);
			}
		}
		else {
			unsigned short dot_checker = 0;
			for (dot_checker = 0; dot_checker < 11; dot_checker++) 
				if (file_to_add->file_name[dot_checker] == '.') {
					printf("Function FAT_directory_add: Invalid file name!");
					free(file_metadata);
					return -1;
				}
			
			file_to_add->creation_date 			= FAT_current_date();
			file_to_add->creation_time 			= FAT_current_time();
			file_to_add->creation_time_tenths 	= FAT_current_time_temths();
			file_to_add->last_accessed 			= file_to_add->creation_date;
			file_to_add->last_modification_date = file_to_add->creation_date;
			file_to_add->last_modification_time = file_to_add->creation_time;

			unsigned int new_cluster = FAT_allocate_free();
			if ((new_cluster == BAD_CLUSTER_32 && fat_type == 32) || (new_cluster == BAD_CLUSTER_16 && fat_type ==16) || (new_cluster == BAD_CLUSTER_12 && fat_type == 12)) {
				printf("Function FAT_directory_add: allocation of new cluster failed. Aborting...\n");
				free(file_metadata);
				return -1;
			}

			file_to_add->low_bits = GET_ENTRY_LOW_BITS(new_cluster);
			file_to_add->high_bits = GET_ENTRY_HIGH_BITS(new_cluster);

			memcpy(file_metadata, file_to_add, sizeof(directory_entry_t));
			if (FAT_cluster_write((void *)DISK_WRITE_LOCATION, bytes_per_sector * sectors_per_cluster, 0, cluster) != 0) {
				printf("Function FAT_directory_add: Writing new directory entry failed. Aborting...\n");
				free(file_metadata);
				return -1;
			}

			free(file_metadata);
			return 0;
		}
	}

	free(file_metadata);
	return -1; //return error.
}

//retrieves a specified file from the File System (readInOffset is in clusters)
//Returns: -1 is general error, -2 is directory not found, -3 is path specified is a directory instead of a file
int FAT_get_file(const char* filePath, char** fileContents, directory_entry_t* fileMeta, unsigned int readInOffset) {
	if (fileContents == NULL || fileMeta == NULL) {
		printf("Function FAT_get_file: an argument was invalid!\n");
		return -1;
	}

	char fileNamePart[256]; //holds the part of the path to be searched
	unsigned short start = 0; //starting at 3 to skip the "C:\" bit
	unsigned int active_cluster;
	if (fat_type == 32) active_cluster = ext_root_cluster; //holds the cluster to be searched for directory entries related to the path
	else {
		printf("Function FAT_get_file: FAT16 and FAT12 are not supported!\n");
		return -1;
	}

	directory_entry_t file_info;
	for (unsigned int iterator = 0; iterator <= strlen(filePath); iterator++) {
		if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
			memset(fileNamePart, '\0', 256);
			memcpy(fileNamePart, filePath + start, iterator - start);

			int retVal = FAT_directory_search(fileNamePart, active_cluster, &file_info, NULL);
			if (retVal == -2) return -2;
			else if (retVal == -1) {
				printf("Function FAT_get_file: An error occurred in FAT_directory_search. Aborting...\n");
				return retVal;
			}

			start = iterator + 1;
			active_cluster = GET_CLUSTER_FROM_ENTRY(file_info);
		}
	}

	*fileMeta = file_info;
	if ((file_info.attributes & FILE_DIRECTORY) != FILE_DIRECTORY) {
		if (readInOffset < 1 || (readInOffset * (unsigned short)bytes_per_sector * (unsigned short)sectors_per_cluster) + file_info.file_size > 262144)
			return -3;

		int cluster = GET_CLUSTER_FROM_ENTRY(file_info);
		unsigned int clusterReadCount = 0;

        char* content;
		while (cluster < END_CLUSTER_32) {
			char* content_part = FAT_cluster_read(cluster, clusterReadCount + readInOffset);
            char* new_content = (char*)malloc(strlen(content) + strlen(content_part));
            new_content = strcat(content, content_part);

            free(content);
            content = (char*)malloc(strlen(content) + strlen(content_part));
            strcpy(content, new_content);

            free(new_content);

			clusterReadCount++;
			cluster = FAT_read(cluster);
			if (cluster == BAD_CLUSTER_32) {
				printf("Function FAT_get_file: the cluster chain is corrupted with a bad cluster. Aborting...\n");
				return -1;
			}
			else if (cluster == -1 ) {
				printf("Function FAT_get_file: an error occurred in FAT_read. Aborting...\n");
				return -1;
			}
		}
		
		*fileContents = content;

		return 0; //file successfully found
	}
	else return -4; //the path specified was a directory
}

// TODO: FIX
//writes a new file to the file system
//filepath: specifies the path to where the file will be written
//filecontents: contains the char array to the data that will be written
//fileMeta: contains the metadata that will be written. struct should only have a file name, attributes, and size. the rest will be filled in automatically
//returns: -1 is general error, -2 indicates a bad path/file name, -3 indicates file with same name already exists, -4 indicates file size error
int FAT_put_file(const char* filePath, char** fileContents, directory_entry_t* fileMeta) {
	if (FAT_name_check(fileMeta->file_name) != 0) {
		printf("\nFunction FAT_put_file: Invalid file name!\n");
		return -2;
	}

	char fileNamePart[256]; //holds the part of the path to be searched
	unsigned short start = 0; //starting at 3 to skip the "C:\" bit
	unsigned int active_cluster; //holds the cluster to be searched for directory entries related to the path

	if (fat_type == 32) active_cluster = ext_root_cluster;
	else {
		printf("Function FAT_put_file: FAT16 and FAT12 are not supported!\n");
		return -1;
	}

	directory_entry_t file_info; //holds found directory info
	if (strlen(filePath) == 0) {
		if (fat_type == 32) {
			active_cluster = ext_root_cluster;
			file_info.attributes = FILE_DIRECTORY | FILE_VOLUME_ID;
			file_info.file_size = 0;
			file_info.high_bits = GET_ENTRY_HIGH_BITS(active_cluster);
			file_info.low_bits = GET_ENTRY_LOW_BITS(active_cluster);
		}
		else {
			printf("Function FAT_put_file: FAT16 and FAT12 are not supported!\n");
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
						printf("Function FAT_put_file: No matching directory found. Aborting...\n");
					return -2;

					case -1:
						printf("Function FAT_put_file: An error occurred in FAT_directory_search. Aborting...\n");
					return retVal;
				}

				start = iterator + 1;
				active_cluster = GET_CLUSTER_FROM_ENTRY(file_info); //prep for next search
			}
	}

	//directory to receive the file is now found, and its cluster is stored in active_cluster. Search the directory to ensure the specified file name is not already in use
	char output[13];
	FAT_fatname2name((char *)fileMeta->file_name, output);
	
	int retVal = FAT_directory_search(output, active_cluster, NULL, NULL);
	if (retVal == -1) {
		printf("Function putFile: directorySearch encountered an error. Aborting...\n");
		return -1;
	}
	else if (retVal != -2) {
		printf("Function putFile: a file matching the name given already exists. Aborting...\n");
		return -3;
	}

	if ((file_info.attributes & FILE_DIRECTORY) == FILE_DIRECTORY) {
		if (FAT_directory_add(active_cluster, fileMeta) != 0) {
			printf("Function FAT_put_file: FAT_directory_add encountered an error. Aborting...\n");
			return -1;
		}

		//now filling file_info with the information of the file directory entry
		char output[13];
		FAT_fatname2name((char *)fileMeta->file_name, output);

		int retVal = FAT_directory_search(output, active_cluster, &file_info, NULL);
		if (retVal == -2) {
			printf("Function FAT_put_file: FAT_directory_add did not properly write the new file's entry to disk. Aborting...\n");
			return -2;
		}
		else if (retVal != 0) {
			printf("Function FAT_put_file: FAT_directory_search has encountered an error. Aborting...\n");
			return -1;
		}

		active_cluster = GET_CLUSTER_FROM_ENTRY(file_info);
		unsigned int dataLeftToWrite = fileMeta->file_size;

		//start writing information to disk
		while (dataLeftToWrite > 0) {
			unsigned int dataWrite = 0;
			if (dataLeftToWrite >= bytes_per_sector * sectors_per_cluster) dataWrite = bytes_per_sector * sectors_per_cluster + 1;
			else dataWrite = dataLeftToWrite;

			//Always offset by at least one, so any file operations happening exactly at DISK_READ_LOCATION (e.g. FAT Table lookups) don't overwrite the data (this is essentially backwards compatibility with previously written code)
			if (FAT_cluster_write(*fileContents + (fileMeta->file_size - dataLeftToWrite), dataWrite, 1, active_cluster) != 0) {
				printf("Function FAT_put_file: FAT_cluster_write encountered an error. Aborting...\n");
				return -1;
			}

			dataLeftToWrite -= dataWrite; //subtract the bytes that were just written

			//if there's no data left to write, exit
			if (dataLeftToWrite == 0) break;
			else if (dataLeftToWrite < 0) {
				printf("Function FAT_put_file: An undefined value has been detected. Aborting...\n");
				return -1;
			}

			//there's more data to write, so allocate new cluster, change fat of current cluster to point to new cluster, and change active cluster to new cluster

			unsigned int new_cluster = FAT_allocate_free();

			if ((new_cluster == BAD_CLUSTER_32 && fat_type == 32) || (new_cluster == BAD_CLUSTER_16 && fat_type == 16) || (new_cluster == BAD_CLUSTER_12 && fat_type == 12)) {
				printf("Function FAT_put_file: FAT_allocate_free encountered an error. Aborting...\n");
				return -1;
			}
			if (FAT_write(active_cluster, new_cluster) != 0) {
				printf("Function FAT_put_file: FAT_write encountered an error. Aborting...\n");
				return -1;
			}

			active_cluster = new_cluster;
		}

		return 0; //file successfully written
	}
	else {
		printf ("Function FAT_put_file: Invalid path!\n");
		return -2; //invalid path!
	}
}

//clock hasn't been implemented yet
unsigned short FAT_current_time() {
	datetime_read_rtc();
	return datetime_hour;
}

//clock nor date has been implemented yet
unsigned short FAT_current_date() {
	datetime_read_rtc();
	return datetime_day;
}

//clock hasn't been implemented yet
unsigned char FAT_current_time_temths() {
	datetime_read_rtc();
	return datetime_hour;
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
	unsigned int counter = 0;

	str_uppercase(input);

	char searchName[13] = { '\0' };
	unsigned short dotPos = 0;

	counter = 0;
	while (counter <= 8) {
		if (input[counter] == '.' || input[counter] == '\0') {
			dotPos = counter;
			counter++; //iterate off dot
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
		if (input[counter] != '\0') searchName[extCount] = input[counter];
		else searchName[extCount] = ' ';

		counter++;
		extCount++;
	}

	counter = dotPos; //reset counter to position of the dot

	while (counter < 8) {
		searchName[counter] = ' ';
		counter++;
	}

	strcpy(input, searchName); //copy results back to input

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

		if (input[iterator] >= 'a' && input[iterator] <= 'z') {
			retVal = retVal | LOWERCASE_ISSUE;
		}
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

/* description: pass in a entry and this properly formats the
 * "firstCluster" from the 2 byte segments in the file structure
 */
uint32_t FAT_build_cluster_address(directory_entry_t* entry) {
    uint32_t addr = 0x00000000;

    addr |=  entry->high_bits << (fat_type / 2);
    addr |=  entry->low_bits;
    return addr;
}

// createEntry(&newDirEntry, dirName, extention, TRUE, beginNewDirClusterChain, 0, FALSE, FALSE);
/* description: takes a directory entry and all the necesary info
	and populates the entry with the info in a correct format for
	insertion into a disk.
*/
int FAT_create_entry(directory_entry_t* entry, const char* filename,  const char* ext, int isDir, uint32_t firstCluster, uint32_t filesize) {
    entry->reserved0 				= 0; 
	entry->creation_time_tenths 	= 0;
	entry->creation_time 			= 0;
	entry->creation_date 			= 0;
	entry->last_modification_date 	= 0;

	strcpy(entry->file_name, filename);

    if (ext) {
    	strcat(entry->file_name, " ");
    	strcat(entry->file_name, ext);
    }
    
    entry->low_bits = firstCluster;
	entry->high_bits = firstCluster >> 16;  

    if(isDir == TRUE) {
        entry->file_size = 0;
        entry->attributes = FILE_DIRECTORY;
	} else {
        entry->file_size = filesize;
        entry->attributes = FILE_ARCHIVE;
	}

    return 0; 
}