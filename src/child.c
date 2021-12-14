#include <stdio.h>
#include <stdbool.h>
#include "unistd.h"

typedef enum{
	reading_suc,
	reading_eol,
	reading_wrong_value,
	reading_eof,
} read_rvl_stat;

read_rvl_stat reading_float(int st, float* cur){
	bool dot_fnd = false;
	bool min_fnd = false;
	char c;
	*cur = 0;
	double i = 0.1;
	int k = read(st, &c, sizeof(char));				// считывает число 1 чаром
	while(k > 0){
		if(c == '\n'){						// проверка на то, что был считан символ конца строки
			return reading_eol;
		}
		if(c == ' '){						// проверка на то, что был считан пробел 
			if(min_fnd){
				*cur = *cur * -1;
			}
			break;
		}
		if(((c < '0') || (c > '9')) && c != '.' && c != '-'){	// проверка на то, что был считан неправильный символ
			return reading_wrong_value;
		}
		if(c == '-'){
			min_fnd = true;
		}
		if(!dot_fnd){						// проверка на то, была ли уже считана точка
			if(c == '.'){					// если не была, то запоминаем
				dot_fnd = true;
			}
			else{						// считаем целую часть числа
				if (c != '-'){
					*cur = *cur * 10 + c - '0';
				}
			}
		} 
		else{
			if(c == '.'){
				return reading_wrong_value;
			}
			*cur = *cur + i * (c - '0');			// иначе, считаем дробную часть
			i /= 10;					// и уменьшаем степень для следующей цифры
		}
		k = read(st, &c, sizeof(char));				// читаем следующий 1 чар
	}
	if(k == 0){							// если видим 0 - файл закончился
		return reading_eof;
	}
	return reading_suc;
}

int main(){
	float sum = 0.0f;
	float cur = 0.0f;
	read_rvl_stat status = reading_float(0, &cur);
	while(status != reading_wrong_value){
		sum += cur;
		if(status == reading_eof){
			perror("Test should end with <endline>.\n");
			return -1;
		} 
		else if(status == reading_eol){
			break;
		}
		cur = 0.0f;
		status = reading_float(0, &cur);
	}
	if (status == reading_wrong_value){
		perror("Wrong value in test_file.\n");
		return -2;
	}
	write(1, &sum, sizeof(float));
	return 0;
}
