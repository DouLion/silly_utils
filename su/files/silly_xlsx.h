/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-27
 * @file: silly_xlsx.h
 * @description: silly_xlsx 类声明
 * @version: v1.0.1 2024-12-27 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_XLSX_H
#define SILLY_UTILS_SILLY_XLSX_H

namespace silly
{
class xlsx
{
  public:
    xlsx();
    ~xlsx();

  private:
};
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_XLSX_H

/* https://github.com/brechtsanders/xlsxio/blob/master/examples/example_xlsxio_write.c

 #include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "xlsxio_write.h"

const char* filename = "example.xlsx";

int main (int argc, char* argv[])
{
   xlsxiowriter handle;
   //open .xlsx file for writing (will overwrite if it already exists)
   if ((handle = xlsxiowrite_open(filename, "MySheet")) == NULL) {
       fprintf(stderr, "Error creating .xlsx file\n");
       return 1;
   }
   //set row height
   xlsxiowrite_set_row_height(handle, 1);
   //how many rows to buffer to detect column widths
   xlsxiowrite_set_detection_rows(handle, 10);
   //write column names
   xlsxiowrite_add_column(handle, "Col1", 0);
   xlsxiowrite_add_column(handle, "Col2", 21);
   xlsxiowrite_add_column(handle, "Col3", 0);
   xlsxiowrite_add_column(handle, "Col4", 2);
   xlsxiowrite_add_column(handle, "Col5", 0);
   xlsxiowrite_add_column(handle, "Col6", 0);
   xlsxiowrite_add_column(handle, "Col7", 0);
   xlsxiowrite_next_row(handle);
   //write data
   int i;
   for (i = 0; i < 1000; i++) {
       xlsxiowrite_add_cell_string(handle, "Test");
       xlsxiowrite_add_cell_string(handle, "A b  c   d    e     f\nnew line");
       xlsxiowrite_add_cell_string(handle, "&% <test> \"'");
       xlsxiowrite_add_cell_string(handle, NULL);
       xlsxiowrite_add_cell_int(handle, i);
       xlsxiowrite_add_cell_datetime(handle, time(NULL));
       xlsxiowrite_add_cell_float(handle, 3.1415926);
       xlsxiowrite_next_row(handle);
   }
   //close .xlsx file
   xlsxiowrite_close(handle);
   return 0;
}
 *
 */

/* https://github.com/brechtsanders/xlsxio/blob/master/examples/example_xlsxio_read.c
 *
 * #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if _WIN32
#include <windows.h>
#else
#define O_BINARY 0
#endif
#ifdef PROCESS_FROM_MEMORY
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#ifdef PROCESS_FROM_FILEHANDLE
//#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#include "xlsxio_read.h"

#if !defined(XML_UNICODE_WCHAR_T) && !defined(XML_UNICODE)
//UTF-8 version
#define X(s) s
#define XML_Char_printf printf
#else
//UTF-16 version
#define X(s) L##s
#define XML_Char_printf wprintf
#endif

const char* filename = "example.xlsx";

int main (int argc, char* argv[])
{
#ifdef _WIN32
    //switch Windows console to UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc > 1)
        filename = argv[1];

    xlsxioreader xlsxioread;

    XML_Char_printf(X("XLSX I/O library version %s\n"), xlsxioread_get_version_string());

#if defined(PROCESS_FROM_MEMORY)
    {
        //load file in memory
        int filehandle;
        char* buf = NULL;
        size_t buflen = 0;
        if ((filehandle = open(filename, O_RDONLY | O_BINARY)) != -1) {
            struct stat fileinfo;
            if (fstat(filehandle, &fileinfo) == 0) {
                if ((buf = malloc(fileinfo.st_size)) != NULL) {
                    if (fileinfo.st_size > 0 && read(filehandle, buf, fileinfo.st_size) == fileinfo.st_size) {
                        buflen = fileinfo.st_size;
                    }
                }
            }
            close(filehandle);
        }
        if (!buf || buflen == 0) {
            fprintf(stderr, "Error loading .xlsx file\n");
            return 1;
        }
        if ((xlsxioread = xlsxioread_open_memory(buf, buflen, 1)) == NULL) {
            fprintf(stderr, "Error processing .xlsx data\n");
            return 1;
        }
    }
#elif defined(PROCESS_FROM_FILEHANDLE)
    //open .xlsx file for reading
    int filehandle;
    if ((filehandle = open(filename, O_RDONLY | O_BINARY, 0)) == -1) {
        fprintf(stderr, "Error opening .xlsx file\n");
        return 1;
    }
    if ((xlsxioread = xlsxioread_open_filehandle(filehandle)) == NULL) {
        fprintf(stderr, "Error reading .xlsx file\n");
        return 1;
    }
#else
    //open .xlsx file for reading
    if ((xlsxioread = xlsxioread_open(filename)) == NULL) {
        fprintf(stderr, "Error opening .xlsx file\n");
        return 1;
    }
#endif

    //list available sheets
    xlsxioreadersheetlist sheetlist;
    const XLSXIOCHAR* sheetname;
    printf("Available sheets:\n");
    if ((sheetlist = xlsxioread_sheetlist_open(xlsxioread)) != NULL) {
        while ((sheetname = xlsxioread_sheetlist_next(sheetlist)) != NULL) {
            XML_Char_printf(X(" - %s\n"), sheetname);
        }
        xlsxioread_sheetlist_close(sheetlist);
    }

    //read values from first sheet
    XLSXIOCHAR* value;
    printf("Contents of first sheet:\n");
    xlsxioreadersheet sheet = xlsxioread_sheet_open(xlsxioread, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);
    while (xlsxioread_sheet_next_row(sheet)) {
        while ((value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
            XML_Char_printf(X("%s\t"), value);
            xlsxioread_free(value);
        }
        printf("\n");
    }
    xlsxioread_sheet_close(sheet);

    //clean up
    xlsxioread_close(xlsxioread);
    return 0;
}

    */