#ifndef CONFIG_H
#define CONFIG_H
/*-- Define --*/
#define MAX_LINE_SIZE 1048576 // Максимальная длина одной строки в файле 2^20
/*-- Variables --*/
enum CsvstatExitCode
    {
     CSVSTAT_EXIT_OK = 0,
     CSVSTAT_EXIT_ERROR = 1,
     CSVSTAT_EXIT_BAD_CLI = 2
    };//Внешние коды завершения программы заданы условием лабораторной.

#endif

