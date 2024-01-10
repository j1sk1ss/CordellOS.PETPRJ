#include "../../include/elf.h"

void** ELF_read(const char* path) {
    uint8_t* headerBuffer;
    uint32_t filePos = 0;

    struct FATContent* content = FAT_get_content(path);
    uint8_t* file_content = content->file->data;

    // Read header
    headerBuffer = (uint8_t*)malloc(sizeof(ELFHeader));
    memcpy(headerBuffer, file_content, sizeof(ELFHeader));

    filePos += sizeof(ELFHeader);
    file_content += sizeof(ELFHeader);

    // validate header
    bool ok = true;
    ELFHeader* header = (ELFHeader*)headerBuffer;

    ok = ok && (memcmp(header->Magic, ELF_MAGIC, 4) != 0);
    ok = ok && (header->Bitness == ELF_BITNESS_32BIT);
    ok = ok && (header->Endianness == ELF_ENDIANNESS_LITTLE);
    ok = ok && (header->ELFHeaderVersion == 1);
    ok = ok && (header->ELFVersion == 1);
    ok = ok && (header->Type == ELF_TYPE_EXECUTABLE);
    ok = ok && (header->InstructionSet == ELF_INSTRUCTION_SET_X86);

    printf("IS ELF (0): [%i]\tENDIANESS (1): [%u]\t ELF HEADER VER (1): [%u]\n", memcmp(header->Magic, ELF_MAGIC, 4), header->Endianness, header->ELFHeaderVersion);
    printf("TYPE (2): [%u]\tINSTRUCTIONS (3): [%u]\t ELF VERSION (1): [%u]\n", header->Type, header->InstructionSet, header->ELFVersion);
    printf("HEADER OFFSET: [%u]\tHEADER SIZE: [%u]\t ENTRY SIZE : [%u]\n", header->ProgramHeaderTablePosition, header->ProgramHeaderTableEntrySize, header->ProgramHeaderTableEntrySize);
    printf("TABLE COUNT: [%u]\tBITNESS: [%u]\n", header->ProgramHeaderTableEntryCount, header->Bitness);
    return NULL;

    void** entryPoint = (void*)header->ProgramEntryPosition;

    // load program header
    uint32_t programHeaderOffset            = header->ProgramHeaderTablePosition;
    uint32_t programHeaderSize              = header->ProgramHeaderTableEntrySize * header->ProgramHeaderTableEntryCount;
    uint32_t programHeaderTableEntrySize    = header->ProgramHeaderTableEntrySize;
    uint32_t programHeaderTableEntryCount   = header->ProgramHeaderTableEntryCount;

    free(headerBuffer);
    file_content += programHeaderOffset - filePos;

    headerBuffer = malloc(programHeaderSize);
    memcpy(headerBuffer, file_content, programHeaderSize);

    filePos += programHeaderSize;
    file_content += programHeaderSize;

    // Reset pointer
    file_content = content->file->data;

    // parse program header entries
    for (uint32_t i = 0; i < programHeaderTableEntryCount; i++) {
        ELFProgramHeader* progHeader = (ELFProgramHeader*)(headerBuffer + i * programHeaderTableEntrySize);
        if (progHeader->Type == ELF_PROGRAM_TYPE_LOAD) {
            uint8_t* virtAddress = (uint8_t*)progHeader->VirtualAddress;
            memset(virtAddress, 0, progHeader->MemorySize);
            
            // Seek header
            file_content += progHeader->Offset;

            // read program
            memcpy(virtAddress, file_content, progHeader->FileSize);
        }
    }

    FAT_unload_content_system(content);
    return entryPoint;
}