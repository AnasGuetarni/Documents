#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <math.h>
#include "ppm.h"
#include <pthread.h>

#define FIRST_PIXEL 11
#define BITS_PER_CHAR 7

typedef struct limit_threads_st {
	int initial_indice;
	int initial_pos_rgb;
} limit_threads_t;

typedef struct param_st {
	int char_interval;
	limit_threads_t limit;
	img_t *img;
} param_t;

int add_pow_2(uint8_t *ptr, int exp)
{
	return (*ptr % 2) * pow(2, exp);
}

int get_nb_char_img(img_t *img)
{
	uint8_t *test_rgb = &img->raw[0].r;
	int nb_char=0, count=0;

	for (int i = sizeof(int)*8-1; i >= 0; i--)
	{
		nb_char += add_pow_2(test_rgb+i, count);
		count++;
	}

	return nb_char;
}

limit_threads_t get_limits(int min){
	limit_threads_t ret;
	
	ret.initial_indice = floor((float)(min * BITS_PER_CHAR) / sizeof(pixel_t)) + FIRST_PIXEL;
	ret.initial_pos_rgb = (min * BITS_PER_CHAR) % sizeof(pixel_t);

	return ret;
}

void *thread(void *para){
    param_t *p = (param_t *)para;
    int initial_ind = p->limit.initial_indice;
    int initial_pos = p->limit.initial_pos_rgb;
    img_t *img = p->img;
	uint8_t *ptr = &img->raw[initial_ind].r + initial_pos;
    char *str_return = calloc(p->char_interval+1, sizeof(char));

    for (int i = 0; i < p->char_interval; i++)
    {
    	char tmp_char = 0;
    	for (int indice = BITS_PER_CHAR-1; indice >= 0; indice--)
    	{
    		tmp_char += add_pow_2(ptr, indice);
    		ptr++;
    	}
		str_return[i] = tmp_char;
    }

    return (void *) str_return;
}



int main(int argc, char **argv){



	img_t *img;
	img=load_ppm("./img_out.ppm");
	int nb_threads = 50;
	int nb_char = get_nb_char_img(img);
	float interval = (float)nb_char / (float)nb_threads;
	int count_thread=0;
	void *str_return;
	char *text_decoded = calloc(nb_char + 1, sizeof(char));
	param_t *threads_param = malloc(sizeof(param_t) * nb_threads);
	pthread_t *threads = malloc(sizeof(pthread_t) * nb_threads);

	for (int i = 0; i < nb_threads; i++)
	{
		int min = round(interval * i) + 0;
		int max = round(interval * (i + 1)) - 1;
		int char_in_interval = max + 1 - min;

		if (char_in_interval > 0)
		{
			threads_param[count_thread].limit = get_limits(min);
			threads_param[count_thread].char_interval = char_in_interval;
			threads_param[count_thread].img = img;

			int code = pthread_create(&threads[count_thread], NULL, thread, &threads_param[count_thread]);

			if (code != 0){
	            fprintf(stderr, "pthread_create failed!\n");
	            exit(0);
			}
				
			count_thread++;
		}
	}

	for (int i = 0; i < count_thread; i++)
    {
        pthread_join(threads[i], &str_return);

        // if (i==0)
        // 	strcpy(text_decoded, str_return);
        // else
        	strcat(text_decoded, str_return);
        
    }

    printf("/////////////////////////// TEXT DECODED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n\n%s\n\n", text_decoded);
}
