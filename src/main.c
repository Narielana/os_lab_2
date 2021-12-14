#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "unistd.h"

const int MaxBuf = 4096;

int main(){
	char* file_name;
	file_name = malloc(sizeof(char)*MaxBuf);			// выделяется временная память для имени файла
	if (read(0, file_name, MaxBuf) < 0){			// читается имя файла со стандартного ввода
		perror("Cannot read a name of the file.\n");
		return 1;
	}
	int l1 = strlen(file_name);				// находим длину строки
	file_name[l1 - 1] = '\0';				// добавляется \0 т.к. он должен быть в конце массива чаров
	fflush(stdout);						// очищается стандартный вывод
	int file = open(file_name, O_RDONLY);			// открывает файл на чтение, open() возвращает дескриптор (0 - стандартный ввод, 1 - стандартный вывод, 
	if(file == -1){						// прочие - какие-то файловые дескрипторы, -1 - всё плохо)
		perror("File cannot be opened.");
		free(file_name);				// очищает место в памяти (строку с именем файла)
		return -5;
	}
	free(file_name);					// ^ т.к. оно больше не нужно, ведь есть дескриптор
	int fd[2];						// к любому pipe привязаны два числа (0 - конец канала чтения, 1 - конец канала записи)
	if(pipe(fd) < 0){					// создаёт pipe
		perror("Cannot create a pipe.");
		return -2;
	}
	int pid = fork();					// создаёт новый процесс и запоминает индентификатор процесса
	if(pid == -1){
		perror("Cannot create a fork.");
		return -1;
	}
	if(pid == 0){						// дочерний процесс получает идентификатор 0
		close(fd[0]);
		if(dup2(file, 0) < 0){				// перенаправляет стандартный поток ввода
			perror("Cannot dup fd[0] to stdin.");
			return -5;
		}
		if(dup2(fd[1], 1) < 0){				// перенаправляет стандартный поток вывода
			perror("Cannot dup fd[1] to stdout.");
			return -6;
		}
		if(execl("child", "", NULL) == -1){		// запуск дочерней программы
			perror("Execl child problem.");
			return -7;
		}
	} 
	else{							// процесс-родитель получает идентификатор > 0
		int status;
		wait(&status);					// ожидает изменение статуса дочернего процесса
		if(WEXITSTATUS(status)){
			return -1;
		}
		float res = 0.0f;
		read(fd[0], &res, sizeof(float));
		printf("%f\n", res);
		close(fd[0]);
		close(fd[1]);
	}
	return 0;
}
