/*************************************************************************
	> File Name: s5pv210_addhead_in_bl1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年01月04日 星期三 12时24分28秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SPL_FILE    "spl/u-boot-spl.bin"
#define UBOOT_FILE  "u-boot.bin"
#define DST_FILE    "mini2440_uboot.bin"

#define IMG_SIZE        1 * 1024 * 1024

int main(int argc, char *argv[])
{
    unsigned char *chFileBuffer;
    FILE *fp;
    int bufferLen;
    int nByte, fileLen;
    unsigned int CheckSum, Count;
    int i;

    
    chFileBuffer = calloc(1, IMG_SIZE);
    if (NULL == chFileBuffer) {
        perror("chFileBuffer calloc fail! \n");
        return -1;
    } 

    fp = fopen(SPL_FILE, "rb");
    if (NULL == fp) {
        printf("open spl file %s fail!\n", SPL_FILE);
        free(chFileBuffer);
        return -1;
    }
        
    fseek(fp, 0L, SEEK_END);
    fileLen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    printf("%d byte of %s!\n", fileLen, SPL_FILE);
    if (fileLen > (IMG_SIZE)) {
        fprintf(stderr, "Source file is too big(>16Kbyte)!\n");
        free(chFileBuffer);
        fclose(fp);
    }
    
    i = 0;
    while(fread((chFileBuffer + i), 1, 1, fp)) {
        i++;
    }
    printf("Read %s end\n", SPL_FILE);
    fclose(fp);

    fp = fopen(UBOOT_FILE, "rb");
    if (NULL == fp) {
        printf("open spl file %s fail!\n", UBOOT_FILE);
        free(chFileBuffer);
        return -1;
    }
        
    fseek(fp, 0L, SEEK_END);
    fileLen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    printf("%d byte of %s!\n", fileLen, SPL_FILE);
    if (fileLen > (IMG_SIZE - 4096)) {
        fprintf(stderr, "Source file is too big(>1020Kbyte)!\n");
        free(chFileBuffer);
        fclose(fp);
    }
    
    i = 0;
    while(fread((chFileBuffer + 4096 + i), 1, 1, fp)) {
        i++;
    }
    printf("Read %s end\n", UBOOT_FILE);
    fclose(fp);


    fp = fopen(DST_FILE, "wb");
    if (NULL == fp) {
        perror("open distination file error!\n");
        free(chFileBuffer);
        return -1;
    }

    nByte = fwrite(chFileBuffer, 1, fileLen + 4096, fp);
    if (nByte != fileLen + 4096) {
        perror("distination file write error!\n");
        free(chFileBuffer);
        fclose(fp);
        return -1;
    }
    printf("write %d byte in %s!\n", nByte, DST_FILE);
    free(chFileBuffer);
    fclose(fp);
    printf("write successful!\n");
    return 0;
}
