#define HEADER_FILE "out/header.txt"
#define SECTIONS_FILE "out/sections.txt"
#define CODE_FILE "out/code.txt"

#include <wtypes.h>
#include <winnt.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

#include "util.h"
#include "errors.h"

void usage(){
    fprintf( stderr, "Usage: ./pereader your-exe.exe\n");
}

void print_dos_header(IMAGE_DOS_HEADER* dosHeader, FILE* f){
	fprintf(f ,"******* DOS HEADER *******\n");
	fprintf(f ,"e_magic\t\t0x%x\n", dosHeader->e_magic);
	fprintf(f ,"e_cblp\t\t0x%x\n", dosHeader->e_cblp);
	fprintf(f ,"e_cp\t\t0x%x\n", dosHeader->e_cp);
	fprintf(f ,"e_crlc\t\t0x%x\n", dosHeader->e_crlc);
	fprintf(f ,"e_cparhdr\t0x%x\n", dosHeader->e_cparhdr);
	fprintf(f ,"e_minalloc\t0x%x\n", dosHeader->e_minalloc);
	fprintf(f ,"e_maxalloc\t0x%x\n", dosHeader->e_maxalloc);
	fprintf(f ,"e_ss\t\t0x%x\n", dosHeader->e_ss);
	fprintf(f ,"e_sp\t\t0x%x\n", dosHeader->e_sp);
	fprintf(f ,"e_csum\t\t0x%x\n", dosHeader->e_csum);
	fprintf(f ,"e_ip\t\t0x%x\n", dosHeader->e_ip);
	fprintf(f ,"e_cs\t\t0x%x\n", dosHeader->e_cs);
	fprintf(f ,"e_lfarlc\t0x%x\n", dosHeader->e_lfarlc);
	fprintf(f ,"e_ovno\t\t0x%x\n", dosHeader->e_ovno);
	fprintf(f ,"e_oemid\t\t0x%x\n", dosHeader->e_oemid);
	fprintf(f ,"e_oeminfo\t0x%x\n", dosHeader->e_oeminfo);
	fprintf(f ,"e_lfanew\t0x%ld\n", dosHeader->e_lfanew);
}

void print_nt_headers(IMAGE_NT_HEADERS* ntHeaders, FILE* f){
    fprintf(f , "******* NT HEADER *******\n");
    fprintf(f , "Signature\t0x%ld\n", ntHeaders->Signature);
}

void print_file_header(IMAGE_FILE_HEADER* fileHeader, FILE* f){
    fprintf(f , "******* FILE HEADER *******\n");
    fprintf(f , "Machine\t\t\t\t\t0x%x\n", fileHeader->Machine);
    fprintf(f , "NumberOfSections\t\t0x%x\n", fileHeader->NumberOfSections);
    fprintf(f , "SizeOfOptionalHeader\t0x%x\n", fileHeader->SizeOfOptionalHeader);
    fprintf(f , "TimeDateStamp\t\t\t0x%lx\n", fileHeader->TimeDateStamp);
    fprintf(f , "NumberOfSymbols\t\t\t0x%lx\n", fileHeader->NumberOfSymbols);
    fprintf(f , "PointerToSymbolTable\t0x%lx\n", fileHeader->PointerToSymbolTable);
    fprintf(f , "Characteristics\t\t\t0x%x\n", fileHeader->Characteristics);
}

void print_section(IMAGE_SECTION_HEADER* sectionHeader, FILE* f){
    fprintf(f , "Name\t\t\t\t%.8s\n", sectionHeader->Name);
    fprintf(f , "PointerToRawData\t\t0x%lx\n", sectionHeader->PointerToRawData);
    fprintf(f , "SizeOfRawData\t\t\t0x%lx\n", sectionHeader->SizeOfRawData);
    fprintf(f , "NumberOfRelocations\t\t0x%x\n", sectionHeader->NumberOfRelocations);
    fprintf(f , "NumberOfLinenumbers\t\t0x%x\n", sectionHeader->NumberOfLinenumbers);
    fprintf(f , "VirtualAddress\t\t\t0x%lx\n", sectionHeader->VirtualAddress);    
    fprintf(f , "Characteristics\t\t\t0x%lx\n\n", sectionHeader->Characteristics);
}

int main( int argc, char** argv ) {
    if (argc != 2) 
        usage();
    if (argc < 2) 
        err(describe_ru[NOT_ENOUGHT_ARGS]);
    if (argc > 2) 
        err(describe_ru[TOO_MANY_ARGS]);

    FILE* exe_file = fopen(argv[1], "rb"); 

    if (exe_file == NULL) 
        err(describe_ru[FILE_READ_ERR]);

    FILE* sections_out = fopen(SECTIONS_FILE, "w");
    FILE* headers_out = fopen(HEADER_FILE, "w");
    FILE* code_out = fopen(CODE_FILE, "w");

    IMAGE_DOS_HEADER dos_header = {0};
    fseek(exe_file, 0, SEEK_SET);
    fread(&dos_header, sizeof(IMAGE_DOS_HEADER), 1, exe_file);
    print_dos_header(&dos_header, headers_out);

    IMAGE_NT_HEADERS nt_headers = {0};
    fseek(exe_file, dos_header.e_lfanew, SEEK_SET);
    fread(&nt_headers, sizeof(IMAGE_NT_HEADERS), 1, exe_file);
    print_nt_headers(&nt_headers, headers_out);

    //IMAGE_OPTIONAL_HEADER opt_header = nt_headers.OptionalHeader;
    IMAGE_FILE_HEADER file_header = nt_headers.FileHeader;
    print_file_header(&file_header, headers_out);

    IMAGE_SECTION_HEADER section_header = {0};
    size_t curr_section = sizeof(IMAGE_FILE_HEADER) + file_header.SizeOfOptionalHeader + dos_header.e_lfanew + sizeof(DWORD);

    fprintf(sections_out , "******* SECTION HEADERS *******\n");
    for (size_t i = 0; i < file_header.NumberOfSections; i++)
    {
        fseek(exe_file, curr_section, SEEK_SET);
        fread(&section_header, sizeof(IMAGE_SECTION_HEADER), 1, exe_file);
        print_section(&section_header, sections_out);
        if (section_header.Characteristics & 0x00000020)
        {
            byte curr_byte;
            size_t curr_pointer = section_header.PointerToRawData;
            for (size_t i = 0; i < section_header.SizeOfRawData; i++)
            {
                fseek(exe_file, curr_pointer, SEEK_SET);
                curr_byte = fgetc(exe_file);
                fwrite(&curr_byte, 1, sizeof(curr_byte), code_out);
                curr_pointer += sizeof(byte);
            }  
        }
        curr_section += sizeof(IMAGE_SECTION_HEADER);
    }
    
    fclose(exe_file);
    fclose(sections_out);
    fclose(headers_out);
    fclose(code_out);
    return 0;
}