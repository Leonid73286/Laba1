#ifndef FUNC_C
#define FUNC_C
#include "func.h"
/*-- Init functions --*/
void ParsingSchema(char *NameFile)
{
 char ***ArrayCSV = NULL;//Объявляем трёхмерный указатель и инициализируем его пустым значением.
                         //Первая * — список строк 
                         //Вторая * — список ячеек внутри строки 
                         //Третья * — текст внутри конкретной ячейки 
 int row_count = 0;//Счётчик row
 int col_count = 0;//Счётчик col
 int current_col;//индекс текущего столбца при разборе строки
 char *token;//адрес строки с запятой
 char line[MAX_LINE_SIZE];//статический текстовый буфер, в который программа временно считывает одну строку из CSV-файла
 int is_double;//Флаг если нашли мы хотя бы одно вещественное число в столбце
 char *col_name;//Для названия столбцов
 //Открываем файл для чтения
 FILE *file = fopen(NameFile, "r");
 if(!file)
   {
    printf("----------------------------------------\n");  
    fprintf(stderr,"csvstat: Attention! Error opening file %s\n",NameFile); 
    return;
   }
 printf("----------------------------------------\n");  
 printf("File %s successfully opened.\n",NameFile);  
 printf("----------------------------------------\n");  
 //Читаем файл построчно
 while(fgets(line, sizeof(line), file)) 
     {
      //Удаляем символ переноса строки \n или \r\n в конце
      line[strcspn(line, "\r\n")] = 0;
      //Выделяем память под новую строку в матрице
      ArrayCSV = realloc(ArrayCSV, (row_count + 1) * sizeof(char **));
      ArrayCSV[row_count] = NULL;
      current_col = 0;
      //Разбиваем строку на токены по запятой
      token = strtok(line, ",");
      while(token != NULL) 
           {
            ArrayCSV[row_count] = realloc(ArrayCSV[row_count], (current_col + 1) * sizeof(char *));
            ArrayCSV[row_count][current_col] = strdup(token); // Копируем строку в массив
            current_col++;
            token = strtok(NULL, ",");
           }
      //Фиксируем количество колонок по первой строке (заголовкам)
      if(row_count == 0)
        {
         col_count = current_col;
        }
      row_count++;
     }
 fclose(file);
 //Выводим заполненный массив на экран для проверки
 printf("\n");
 printf("--- The read CSV Array ---\n");
 for(int i = 0; i < row_count; i++) 
    {
     for(int j = 0; j < col_count; j++)
        {
          printf("%-10s", ArrayCSV[i][j]);
        }
     printf("\n");
    }    
 printf("----------\n");
 //Выводим общую информацию о количестве строк и столбцов
 //row_count - 1, так как первая строка — это заголовок с названиями столбцов
 printf("rows %d\n", row_count);
 printf("columns %d\n", col_count);
 //Анализируем каждый столбец
 for(int j = 0; j < col_count; j++)
    {
     is_double = 0;//если нашли мы хотя бы одно вещественное число в столбце
     for(int i = 1; i < row_count; i++) 
        {
         if(strchr(ArrayCSV[i][j], '.') != NULL)
           {
            is_double = 1;
            break; // Если нашли точку, дальше этот столбец можно не проверять
           }
        }
     //Получаем имя столбца из первой строки (индекс 0)
     col_name = ArrayCSV[0][j];
     //Выводим результат 
     if(is_double)
       {
        printf("%d \"%s\" DOUBLE\n", j, col_name);
       }
     else
         {
          printf("%d \"%s\" INT\n", j, col_name);
         }
    }
 printf("\n");
 //Освобождаем выделенную память    
 for(int i = 0; i < row_count; i++)
    {
     for(int j = 0; j < col_count; j++) 
        {
         free(ArrayCSV[i][j]);
        }
     free(ArrayCSV[i]);
    }
 free(ArrayCSV);
 //
 printf("----------------------------------------\n");
 printf("ParsingSchema(). To exit, press ENTER...\n");
 getchar();//вводит один символ с клавиатуры, просто чтобы окно сразу не закрылось.
}

//
void ParsingStats(char *NameFile)
{
 char ***ArrayCSV = NULL;//Объявляем трёхмерный указатель и инициализируем его пустым значением.
                         //Первая * — список строк 
                         //Вторая * — список ячеек внутри строки 
                         //Третья * — текст внутри конкретной ячейки 
 int row_count = 0;//Счётчик row
 int col_count = 0;//Счётчик col
 int current_col;//индекс текущего столбца при разборе строки
 char *token;//адрес строки с запятой
 char line[MAX_LINE_SIZE];//статический текстовый буфер, в который программа временно считывает одну строку из CSV-файла.
 // Выделяем массивы для хранения метрик каждого столбца
 int *types;//1 - int, 2 - double
 int *nulls;//Счётчик нулевых значений 
 double *mins;//Счётчик min значений  
 double *maxs;//Счётчик max значений   
 double *sums;//Для подсчета среднего
 int *valid_counts;//Количество непустых числовых ячеек
 int *positives;//Счётчик positive     
 int *negatives;//Счётчик negative    
 int *evens;//Счётчик чётных значений      
 int has_double;//Флаг, нашли ли в столбце хоть одну точку 
 int first_numeric; //Флаг для поиска самого первого числа в столбце
 double num_d; //Для преобразования текстовой строки в double 
 char *col_name;//Для названия столбцов
 // Открываем файл для чтения
 FILE *file = fopen(NameFile, "r");
 if(!file)
   {
    printf("----------------------------------------\n");  
    fprintf(stderr,"csvstat: Attention! Error opening file %s\n",NameFile); 
    return;
   }
 printf("----------------------------------------\n");  
 printf("File %s successfully opened.\n",NameFile);  
 printf("----------------------------------------\n");  
 //Читаем файл построчно
 while(fgets(line, sizeof(line), file)) 
     {
      //Удаляем символ переноса строки \n или \r\n в конце
      line[strcspn(line, "\r\n")] = 0;
      //Выделяем память под новую строку в матрице
      ArrayCSV = realloc(ArrayCSV, (row_count + 1) * sizeof(char **));
      ArrayCSV[row_count] = NULL;
      current_col = 0;
      //Разбиваем строку на токены по запятой
      token = strtok(line, ",");
      while(token != NULL) 
           {
            ArrayCSV[row_count] = realloc(ArrayCSV[row_count], (current_col + 1) * sizeof(char *));
            ArrayCSV[row_count][current_col] = strdup(token); // Копируем строку в массив
            current_col++;
            token = strtok(NULL, ",");//Заменяет запятую на \0
           }
      //Фиксируем количество колонок по первой строке (заголовкам)
      if(row_count == 0)
        {
         col_count = current_col;
        }
      row_count++;
     }
 fclose(file);
 //Выводим заполненный массив на экран 
 printf("\n");
 printf("--- The read CSV Array ---\n");
 for(int i = 0; i < row_count; i++) 
    {
     for(int j = 0; j < col_count; j++)
        {
         printf("%-10s", ArrayCSV[i][j]);
        }
     printf("\n");
    }  
  //Сбор статистики по столбцам  
 if(row_count > 1 && col_count > 0) 
   {
    //Выделяем массивы для хранения метрик каждого столбца
     types = malloc(col_count * sizeof(int));       
     nulls = calloc(col_count, sizeof(int));        
     mins = malloc(col_count * sizeof(double));  
     maxs = malloc(col_count * sizeof(double));  
     sums = calloc(col_count, sizeof(double));   
     valid_counts = calloc(col_count, sizeof(int)); 
     positives = calloc(col_count, sizeof(int));    
     negatives = calloc(col_count, sizeof(int));    
     evens = calloc(col_count, sizeof(int));        
     //Определение типа столбца (сканируем со 2-й строки, т.к. 1-я — заголовки)
     for(int j = 0; j < col_count; j++) 
        {
         has_double = 0; // Флаг, нашли ли в столбце хоть одну точку
         for(int i = 1; i < row_count; i++) 
            {
             if(strchr(ArrayCSV[i][j], '.')) 
               {
                has_double = 1;
               }
            }
         //Если нашли хотя бы один double — весь столбец double, иначе int
         types[j] = (has_double) ? 2 : 1;//другая форма оператора if else
        }
     //Сбор информации по столбцам
     for(int j = 0; j < col_count; j++) 
        {
         first_numeric = 1;//Флаг для поиска самого первого числа в столбце
         for(int i = 1; i < row_count; i++) 
            {
             num_d = atof(ArrayCSV[i][j]); //Преобразуем в double для общих расчетов
             valid_counts[j]++;
             sums[j] += num_d;
             //Определение min/max
             if(first_numeric) 
               {
                mins[j] = num_d;
                maxs[j] = num_d;
                first_numeric = 0;
               } 
             else 
                 {
                  if(num_d < mins[j]) mins[j] = num_d;
                  if(num_d > maxs[j]) maxs[j] = num_d;
                 }
             //Определение нулевых значений
             if(num_d == 0.0) 
               {
                nulls[j]++;
               }
             //Число положительных и отрицательных
             if(num_d > 0.0) positives[j]++;
             if(num_d < 0.0) negatives[j]++;
             //Число чётных (только если тип столбца int)
             if(types[j] == 1) 
               {
                long long num_i = atoll(ArrayCSV[i][j]);
                if(num_i % 2 == 0) 
                  {
                   evens[j]++;
                  }
               }
            }
        }

     //Вывод результатов 
     printf("------------------\n"); 
     for(int j = 0; j < col_count; j++) 
        {
         col_name = ArrayCSV[0][j];
         printf("column %d\n", j);
         printf("name \"%s\"\n", col_name);
         if(types[j] == 1) printf("type INT\n");
         else printf("type DOUBLE\n");
         printf("nulls %d\n", nulls[j]);
         if(valid_counts[j] > 0) 
           {
            if(types[j] == 1) 
              {
               printf("min %.0f\n", mins[j]);
               printf("max %.0f\n", maxs[j]);
              } 
            else 
                {
                 printf("min %g\n", mins[j]);
                 printf("max %g\n", maxs[j]);
                }
            printf("mean %f\n", sums[j] / valid_counts[j]);
           } 
         else 
             {
              printf("min -\nmax -\nmean -\n"); 
             }
         printf("positive %d\n", positives[j]);
         printf("negative %d\n", negatives[j]);
         if(types[j] == 1) printf("even %d\n", evens[j]);
         else printf("even -\n");
         printf("\n"); 
        }
     // Освобождаем временные массивы статистики
     free(types); free(nulls); free(mins); free(maxs);
     free(sums); free(valid_counts); free(positives); free(negatives); free(evens);
    }
 //Освобождаем выделенную память    
 for(int i = 0; i < row_count; i++)
   {
    for(int j = 0; j < col_count; j++) 
       {
        free(ArrayCSV[i][j]);
       }
    free(ArrayCSV[i]);
   }
 free(ArrayCSV);
 //
 printf("----------------------------------------\n");
 printf("ParsingStats().To exit, press ENTER...\n");
 getchar();//вводит один символ с клавиатуры, просто чтобы окно сразу не закрылось.
}
//
void  PrintHelp()
{
 printf("______________________________________________________________\n");
 printf("The program was developed by a student Obrezkov Leonid HSE 2026\n");
 printf("Version: 1.4\n");
 printf("Help:\n");
 printf("        --help - Show this reference;\n");
 printf("        schema <csv-file> - The program outputs:\n");
 printf("                                                - Number of data rows;\n");//число строк данных
 printf("                                                - Number of columns;\n");//число столбцов
 printf("                                                - Name and type of each column.\n");//имя и тип кождого столбца
 printf("        stats <csv-file> - Displays general information about the table, followed by column statistics.\n");//выводит общюю информацию о таблице, затем статистики столбцов
 printf("To exit, press ENTER...\n");
 getchar();//вводит один символ с клавиатуры, просто чтобы окно сразу не закрылось.
}


#endif