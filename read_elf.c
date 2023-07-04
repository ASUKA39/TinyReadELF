#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>

void help();
void FileHeader(const char *buf);
void SectionHeader(const char *buf);
void TableHeader(const char *buf);
void ProgramHeader(const char *buf);
void RelocHeader(const char *buf);

int main(int argc, char * argv[]){
    if(argc != 3){
        printf("Wrong arguments\n");
        printf("format: ./read_elf  <option>   file_path\n");
        help();
        return 0;
    }

    int fd = open(argv[2], O_RDONLY, 0);
    if(fd == -1){
        perror("*** file open failed ***\n");
        return 0;
    }

    long int end = lseek(fd, 0, SEEK_END);
    long int begin = lseek(fd, 0, SEEK_SET);
    char* FileBuffer = malloc(end);
    if(FileBuffer == 0){
        perror("*** allocate buffer failed ***\n");
        return 0;
    }

    memset(FileBuffer, 0, end);
    if(read(fd, FileBuffer, end) == -1){
        perror("*** read file failed ***\n");
        return 0;
    }

    if(strcmp(argv[1], "-h") == 0){
        FileHeader(FileBuffer);
    }
    else if(strcmp(argv[1], "-S") == 0){
        SectionHeader(FileBuffer);
    }
    else if(strcmp(argv[1], "-s") == 0){
        TableHeader(FileBuffer);
    }
    else if(strcmp(argv[1], "-l") == 0){
        ProgramHeader(FileBuffer);
    }
    else if(strcmp(argv[1], "-r") == 0){
        RelocHeader(FileBuffer);
    }
    else{
        printf("Wrong arguments\n");
        printf("format: ./read_elf  <option>   file_path\n");
        help();
    }

    return 0;
}

void help(){
    printf("-h  elf header\n");
    printf("-S  section header\n");
    printf("-s  symbol header\n");
    printf("-l  program header\n");
    printf("-r  relocation table\n");
    return;
}

void FileHeader(const char *buf){
    printf("ELF header:\n");

    printf("  Magic:  ");
    for(int i = 0; i < EI_NIDENT; i++){
        printf("%02X", buf[i]);
        printf("  ");
    }
    printf("\n");

    // EI_MAG[0:3]
    if(buf[0] != 0x7f && buf[1] != 'E' && buf[2] != 'L' && buf[3] != 'F'){
        printf("*** Not an ELF file ***\n");
        return;
    }

    // EI_CLASS
    printf("  %-33s:", "Class:");
    switch (buf[4]){
    case 0:
        printf(" Invalid class\n");
        break;
    case 1:
        printf(" ELF32\n");
        break;
    case 2:
        printf(" ELF64\n");
        break;
    default:
        printf(" ERROR\n");
        break;
    }

    // EI_DATA
    printf("  %-33s:", "Data:");
    switch (buf[5]){
    case 0:
        printf(" Invalid data encoding\n");
        break;
    case 1:
        printf(" 2's complement, Little endian\n");
        break;
    case 2:
        printf(" 2's complement, Big endian\n");
        break;
    default:
        printf(" ERROR\n");
        break;
    }

    // version
    printf("  %-33s:", "Version:");
    switch (buf[6]){
    case 0:
        printf(" 0 (None)\n");
        break;
    case 1:
        printf(" 1 (current)\n");
        break;
    default:
        printf(" ERROR\n");
        break;
    }

    // OS/ABI
    printf("  %-33s:", "OS/ABI:");
    switch (buf[7]){
    case 0:
        printf(" Unix - System V\n");
        break;
    case 1:
        printf(" Hewlett-Packard HP-UX\n");
        break;
    case 2:
        printf(" NetBSD\n");
        break;
    case 3:
        printf(" Linux\n");
        break;
    case 6:
        printf(" Sun Solaris\n");
        break;
    case 7:
        printf(" AIX\n");
        break;
    case 8:
        printf(" IRIX\n");
        break;
    case 9:
        printf(" FreeBSD\n");
        break;
    case 10:
        printf(" Compaq TRU64 UNIX\n");
        break;
    case 11:
        printf(" Novell Modesto\n");
        break;
    case 12:
        printf(" Open BSD\n");
        break;
    case 13:
        printf(" Open VMS\n");
        break;
    case 14:
        printf(" Hewlett-Packard Non-Stop Kernel\n");
        break;
    default:
        printf(" other type\n");
        break;
    }

    // ABI version
    printf("  %-33s: %d\n", "ABI version", buf[8]);

    buf += EI_NIDENT;
    // type
    printf("  %-33s:", "Type");
    switch (*(uint16_t*)buf){
        case 0:
            printf(" NONE (No file type)\n");
            break;
        case 1:
            printf(" REL (Relocatable file)\n");
            break;
        case 2:
            printf(" EXEC (Executable file)\n");
            break;
        case 3:
            printf(" DYN (Position-Independent Executable file)\n");
            break;
        case 4:
            printf(" CORE (Core file)\n");
            break;
        case 0xfe00:
            printf(" LOOS (Operating system-specific)\n");
            break;
        case 0xfeff:
            printf(" HIOS (Operating system-specific)\n");
            break;
        case 0xff00:
            printf(" LOPROC (Processor-specific)\n");
            break;
        case 0xffff:
            printf(" HIPROC (Processor-specific)\n");
            break;
        default:
            printf(" ERROR\n");
            break;
    }

    buf += sizeof(uint16_t);
    // Machine
    printf("  %-33s:", "Machine");
    switch(*(uint16_t*)buf){
        case EM_386:
            printf(" Intel 80386\n");
            break;
        case EM_ARM:
            printf(" ARM\n");
            break;
        case EM_X86_64:
            printf(" AMD X86-64 architecture\n");
            break;
        default:
            printf(" other machine\n");
            break;
    }

    buf += sizeof(uint16_t);
    // version
    if(buf == 0){
        printf("  %-33s: %s\n", "version", "invalid version");
    }
    else{
        printf("  %-33s: %s\n", "version", "0X1");
    }

    buf += sizeof(uint32_t);
    // entry start address
    printf("  %-33s: 0X%lx\n", "Entry point address", *(uint64_t*)buf);

    buf += sizeof(uint64_t);
    // Start of program headers
    printf("  %-33s: %lu (bytes into file)\n", "Start of program headers", *(uint64_t*)buf);
    
    buf += sizeof(uint64_t);
    // Start of section headers
    printf("  %-33s: %lu (bytes into file)\n", "Start of section headers", *(uint64_t*)buf);
    
    buf += sizeof(uint64_t);
    // flags
    printf("  %-33s: 0X0\n", "Flags");

    buf += sizeof(uint32_t);
    // Size of this header
    printf("  %-33s: %d (bytes)\n", "Size of this header", *(uint16_t*)buf);
    
    buf += sizeof(uint16_t);
    // size of program headers
    printf("  %-33s: %d (bytes)\n", "Size of program headers", *(uint16_t*)buf);
    
    buf += sizeof(uint16_t);
    // Number of program headers
    printf("  %-33s: %d\n", "Number of program headers", *(uint16_t*)buf);
    
    buf += sizeof(uint16_t);
    // Size of section headers
    printf("  %-33s: %d (bytes)\n", "Size of section headers", *(uint16_t*)buf);
    
    buf += sizeof(uint16_t);
    // Number of section headers
    printf("  %-33s: %d\n", "Number of section headers", *(uint16_t*)buf);
    
    buf += sizeof(uint16_t);
    // Section header string table index
    printf("  %-33s: %d\n", "Section header string table index", *(uint16_t*)buf);

    return;
}

void SectionHeader(const char *buf){
    // Number of section headers
    int NumOfSec = *(uint16_t*)(buf + 0x3c);

    // Section header string table index
    long unsigned int eshoff = *(uint16_t*)(buf + 0x28);

    // Section header string table index
    uint16_t eshstrndx = *(buf + 0x3e);

    // get offset
    Elf64_Shdr* psecheader = (Elf64_Shdr*)(buf + eshoff);
    Elf64_Shdr* pshstr = (Elf64_Shdr*)(psecheader + eshstrndx);
    char* pshstrbuff = (char*)(buf + pshstr->sh_offset);

    printf("There are %d section headers, starting at offset 0x%lx:\n\n", NumOfSec, eshoff);
    printf("Section Headers:\n");
    printf("  [Nr] %-16s  %-16s  %-16s  %-16s\n", "Name", "Type", "Address", "Offset");
    printf("       %-16s  %-16s  %-5s  %-5s %-5s %-5s\n", "Size", "EntSize", "Flags", "Link", "Info", "Align");

    for(int i = 0; i < NumOfSec; i++){
        printf("  [%2d] %-16s  ", i, (char*)(psecheader[i].sh_name + pshstrbuff));
        switch(psecheader[i].sh_type){
            case SHT_NULL:
                printf("%-16s  ", "NULL");break;
            case SHT_PROGBITS:
                printf("%-16s  ", "PROGBITS");break;
            case SHT_SYMTAB:
                printf("%-16s  ", "SYMTAB");break;
            case SHT_STRTAB:
                printf("%-16s  ", "STRTAB");break;
            case SHT_RELA:
                printf("%-16s  ", "RELA");break;
            case SHT_HASH:
                printf("%-16s  ", "GNU_HASH");break;
            case SHT_DYNAMIC:
                printf("%-16s  ", "DYNAMIC");break;
            case SHT_NOTE:
                printf("%-16s  ", "NOTE");break;
            case SHT_NOBITS:
                printf("%-16s  ", "NOBITS");break;
            case SHT_REL:
                printf("%-16s  ", "REL");break;
            case SHT_SHLIB:
                printf("%-16s  ", "SHLIB");break;
            case SHT_DYNSYM:
                printf("%-16s  ", "DYNSYM");break;
            case SHT_INIT_ARRAY:
                printf("%-16s  ", "INIT_ARRY");break;
            case SHT_FINI_ARRAY:
                printf("%-16s  ", "FINI_ARRY");break;
            case SHT_PREINIT_ARRAY:
                printf("%-16s  ", "PREINIT_ARRAY");break;
            case SHT_GNU_HASH:
                printf("%-16s  ", "GNU_HASH");break;
            case SHT_GNU_ATTRIBUTES:
                printf("%-16s  ", "GNU_ATTRIBUTES");break;
            case SHT_GNU_LIBLIST:
                printf("%-16s  ", "GNU_LIBLIST");break;
            case SHT_GNU_verdef:
                printf("%-16s  ", "GNU_verdef");break;
            case SHT_GNU_verneed:
                printf("%-16s  ", "GNU_verneed");break;
            case SHT_GNU_versym:
                printf("%-16s  ", "GNU_versym");break;
            default:
                printf("%-16s  ", "NONE");break;
        }

        printf("%016lX  %08lX\n", psecheader[i].sh_addr, psecheader[i].sh_offset);
        printf("       %016lX  %016lx  ", psecheader[i].sh_size, psecheader[i].sh_entsize);

        switch (psecheader[i].sh_flags) {
            case 0:
                printf("%3s    %4u  %4u     %-4lu\n", "", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 1:
                printf("%3s    %4u  %4u     %-4lu\n", "W", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 2:
                printf("%3s    %4u  %4u     %-4lu\n", "A", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 4:
                printf("%3s    %4u  %4u     %-4lu\n", "X", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 3:
                printf("%3s    %4u  %4u     %-4lu\n", "WA", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 5:
                printf("%3s    %4u  %4u     %-4lu\n", "WX", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 6:
                printf("%3s    %4u  %4u     %-4lu\n", "AX", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case 7:
                printf("%3s    %4u  %4u     %-4lu\n", "WAX", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            case SHF_MASKPROC:
                printf("%3s    %4u  %4u     %-4lu\n", "MS", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            default:
                printf("%3s    %4u  %4u     %-4lu\n", "MS", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
                break;
            }
    }
    printf("Key to Flags:\n");
    printf("  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)\n");
    printf("  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)\n");
    printf("  O (extra OS processing required) o (OS specific), p (processor specific)\n");
    return;
}

void TableHeader(const char *buf){
    // Number of section headers
    int NumOfSec = *(uint16_t*)(buf + 0x3c);

    // Section header string table index
    long unsigned int eshoff = *(uint16_t*)(buf + 0x28);

    // Section header string table index
    uint16_t eshstrndx = *(buf + 0x3e);

    // get offset
    Elf64_Shdr* psecheader = (Elf64_Shdr*)(buf + eshoff);// section header table
    Elf64_Shdr* pshstr = (Elf64_Shdr*)(psecheader + eshstrndx);// section header table + ???
    char* pshstrbuff = (char*)(buf + pshstr->sh_offset);// string table

    for(int i = 0; i < NumOfSec; i++){
        if(strcmp(psecheader[i].sh_name + pshstrbuff, ".dynsym") == 0 || strcmp(psecheader[i].sh_name + pshstrbuff, ".symtab") == 0){
            Elf64_Sym* sym = (Elf64_Sym*)(buf + psecheader[i].sh_offset);
            int count = psecheader[i].sh_size / psecheader[i].sh_entsize;
            char* bufstr = (char*)((psecheader + psecheader[i].sh_link)->sh_offset + buf);
            printf("Symbol table '%s' contains %d entries:\n", psecheader[i].sh_name + pshstrbuff, count);

            printf("%7s  %-8s          %s  %s    %s   %s      %s  %s\n", "Num:", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
            for(int i = 0; i < count; i++)
            {
                printf("%6d:  %016lx  %-6lu", i, sym[i].st_value, sym[i].st_size);
                char type = ELF32_ST_TYPE(sym[i].st_info);
                char bind = ELF32_ST_BIND(sym[i].st_info);
                switch(type)
                {
                    case STT_NOTYPE:
                        printf("%-8s", "NOTYPE");
                        break;
                    case STT_OBJECT:
                        printf("%-8s", "OBJECT");
                        break;
                    case STT_FUNC:
                        printf("%-8s", "FUNC");
                        break;
                    case STT_SECTION:
                        printf("%-8s", "SECTION");
                        break;
                    case STT_FILE:
                        printf("%-8s", "FILE");
                        break;
                    default:
                        break;
                }
                switch(bind)
                {
                    case STB_LOCAL:
                        printf("%-8s", "LOCAL");
                        break;
                    case STB_GLOBAL:
                        printf("%-8s", "GLOBAL");
                        break;
                    case STB_WEAK:
                        printf("%-8s", "WEAK");
                        break;
                    default:
                        break;
                }
                printf("%-8d", sym[i].st_other);
                switch(sym[i].st_shndx)
                {
                    case SHN_UNDEF:
                        printf("%s  %s\n", "UND", sym[i].st_name + bufstr);
                        break;
                    case SHN_ABS:
                        printf("%s  %s\n", "ABS", sym[i].st_name + bufstr);
                        break;
                    case SHN_COMMON:
                        printf("%s  %s\n", "COM", sym[i].st_name + bufstr);
                        break;
                    default:
                        printf("%3d  %s\n", sym[i].st_shndx, sym[i].st_name + bufstr);break;
                }
            }

            continue;
        }
    }

    return;
}

void ProgramHeader(const char *buf){
    Elf64_Ehdr* pfilehead = (Elf64_Ehdr*)buf;
    Elf64_Phdr* pproheader = (Elf64_Phdr*)(buf + pfilehead->e_phoff);
    Elf64_Shdr* psecheader = (Elf64_Shdr*)(buf + pfilehead->e_shoff);
    Elf64_Shdr* pshstr = (Elf64_Shdr*)(psecheader + pfilehead->e_shstrndx);
    char* pstrbuff = (char*)(buf + pshstr->sh_offset);

    printf("Elf file type is\n");
    switch(pfilehead->e_type){
        case 0:
            printf(" No file type\n");
            break;
        case 1:
            printf(" Relocatable file\n");
            break;
        case 2:
            printf(" Executable file\n");
            break;
        case 3:
            printf(" DYN (Position-Independent Executable file)\n");
            break;
        case 4:
            printf(" Core file\n");
            break;
        default:
            printf(" ERROR\n");
            break;
    }

    printf("Entry point 0X%0lX\n", pfilehead->e_entry);
    printf("There are %d  program headers, starting at offset %lu\n\n", pfilehead->e_phnum, pfilehead->e_phoff);
    printf("Program Headers:\n");
    printf("  %-14s  %-16s  %-16s  %-16s\n", "Type", "Offset", "VirtAddr", "PhysAddr");
    printf("  %-14s  %-16s  %-16s  %-6s  %-6s\n", "", "FileSiz", "MemSiz", "Flags", "Align");

    for(int i = 0; i < pfilehead->e_phnum; i++){
        //type
        switch(pproheader[i].p_type){
            case PT_NULL:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_LOAD:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "LOAD", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_DYNAMIC:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "DYNAMIC", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_INTERP:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "INTERP", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_NOTE:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "NOTE", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_SHLIB:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "SHLIB", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_PHDR:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "PHDR", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_TLS:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "TLS", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_NUM:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "NUM", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_GNU_EH_FRAME:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "GNU_EH_FRAME", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_GNU_RELRO:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "GNU_RELRO", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            case PT_GNU_STACK:
                printf("  %-14s  %016lX  %016lX  %016lX\n  %-14s  %016lX  %016lX  ", "GNU_STACK", pproheader[i].p_offset, pproheader[i].p_vaddr, pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);
                break;
            default:
                break;
        }

        switch(pproheader[i].p_flags){
            case PF_X:
                printf("%-6s  0X%-lX\n", "  E", pproheader[i].p_align);break;
            case PF_W:
                printf("%-6s  0X%-lX\n", " W ", pproheader[i].p_align);break;
            case PF_R:
                printf("%-6s  0X%-lX\n", "R  ", pproheader[i].p_align);break;
            case PF_X|PF_W:
                printf("%-6s  0X%-lX\n", " WE", pproheader[i].p_align);break;
            case PF_X|PF_R:
                printf("%-6s  0X%-lX\n", "R E", pproheader[i].p_align);break;
            case PF_W|PF_R:
                printf("%-6s  0X%-lX\n", "RW ", pproheader[i].p_align);break;
            case PF_X|PF_R|PF_W:
                printf("%-6s  0X%-lX\n", "RWE", pproheader[i].p_align);break;
            default:
                printf("\n");
                break;
        }
        if(PT_INTERP == pproheader[i].p_type)
            printf("      [Requesting program interpreter: %s]\n", (char*)(buf + pproheader[i].p_offset));
    }

    printf("\n Section to Segment mapping:\n");
    printf("  Segment Sections...\n");
    for(int i = 0; i < pfilehead->e_phnum; i++){
        printf("   %-7d", i);
        for(int n = 0; n < pfilehead->e_shnum; n++){
            Elf64_Off temp = psecheader[n].sh_addr + psecheader[n].sh_size;
            if((psecheader[n].sh_addr > pproheader[i].p_vaddr && psecheader[n].sh_addr < pproheader[i].p_vaddr + pproheader[i].p_memsz) || (temp > pproheader[i].p_vaddr && temp <= pproheader[i].p_vaddr + pproheader[i].p_memsz)){
                printf("%s ", (char*)(psecheader[n].sh_name + pstrbuff));
            }
        }
        printf("\n");
    }

    return;
}

void RelocHeader(const char *buf){
    Elf64_Ehdr* pfilehead = (Elf64_Ehdr*)buf;
    Elf64_Shdr* psecheader = (Elf64_Shdr*)(buf + pfilehead->e_shoff);
    Elf64_Shdr* pshstr = (Elf64_Shdr*)(psecheader + pfilehead->e_shstrndx);
    char* pstrbuff = (char*)(buf + pshstr->sh_offset);
    for(int i = 0; i < pfilehead->e_shnum; i++){
        if(strncmp(psecheader[i].sh_name + pstrbuff, ".rel", 4) == 0){
            int ncount = psecheader[i].sh_size / psecheader[i].sh_entsize;
            printf("\nRelocation section'%s' at offset %0lX contains %d entries:\n", psecheader[i].sh_name + pstrbuff, psecheader[i].sh_offset, ncount);
            Elf64_Rela* preltable = (Elf64_Rela*)(buf + psecheader[i].sh_offset);

            printf("%-16s  %-16s  %-16s  %-16s  %-16s\n", "Offset", "Info", "Type", "Sym.Value", "Sym.Name + Addend");
            int symnum = psecheader[i].sh_link;
            int strnum = psecheader[symnum].sh_link;

            char* prelstrbuf = (char*)(psecheader[strnum].sh_offset + buf);
            Elf64_Sym* psym = (Elf64_Sym*)(buf + psecheader[symnum].sh_offset);
            for(int n = 0; n < ncount; n++){
                printf("%012lX  %012lX  ", preltable[n].r_offset, preltable[n].r_info);
                switch(ELF64_R_TYPE(preltable[n].r_info)){
                    case R_386_NONE:
                        printf("%-16s", "R_386_NONE");
                        break;
                    case R_386_32:
                        printf("%-16s", "R_386_32");
                        break;
                    case R_386_PC32:
                        printf("%-16s", "R_386_PC32");
                        break;
                    case R_386_GOT32:
                        printf("%-16s", "R_386_GOT32");
                        break;
                    case R_386_PLT32:
                        printf("%-16s", "R_386_PLT32");
                        break;
                    case R_386_COPY:
                        printf("%-16s", "R_386_COPY");
                        break;
                    case R_386_GLOB_DAT:
                        printf("%-16s", "R_386_GLOB_DAT");
                        break;
                    case R_386_JMP_SLOT:
                        printf("%-16s", "R_386_JMP_SLOT");
                        break;
                    case R_386_RELATIVE:
                        printf("%-16s", "R_386_RELATIVE");
                        break;
                    case R_386_GOTOFF:
                        printf("%-16s", "R_386_GOTOFF");
                        break;
                    case R_386_GOTPC:
                        printf("%-16s", "R_386_GOTPC");
                        break;
                    default:
                        break;
                }
                printf("  %016lX  ", (psym + ELF64_R_SYM(preltable[n].r_info))->st_value);

                printf("%s + %lu\n", (char*)(prelstrbuf + (psym + ELF64_R_SYM(preltable[n].r_info))->st_name), preltable[n].r_addend);
            }
        }
    }

    return;
}