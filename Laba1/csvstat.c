#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "func.h"
//Главная функция     
int main(int argc, char *argv[])
{
 if(argc==2 && strcmp(argv[1],"--help")== 0)
   {
     PrintHelp();
     return CSVSTAT_EXIT_OK;
   }
 //
 if(argc==3 && strcmp(argv[1],"schema")== 0 )
   {
     ParsingSchema(argv[2]);
     return CSVSTAT_EXIT_OK;
   }
 //
 if(argc==3 && strcmp(argv[1],"stats")== 0)
   {
     ParsingStats(argv[2]);
     printf("ParsingStats().To exit, press ENTER...\n");
     
     return CSVSTAT_EXIT_OK;
   }
 else
   {
    printf("___________________________________________________________________________\n");
    fprintf(stderr,"csvstat: Attention! The program must be run with parameters! For help --help.\n");
    printf("To continue, press ENTER...\n");
    getchar();//ждём нажатия ENTER, чтобы окно не закрывалось
    return CSVSTAT_EXIT_BAD_CLI;
   }

}
