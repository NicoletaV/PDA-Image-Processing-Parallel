#include "homework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int num_threads;
int resize_factor;

struct helper {
    int thread_id;
    image *in, *out;
};

void readInput(const char * fileName, image *img) {

	FILE* fin = fopen(fileName, "r");
	if(!fin) {
		exit(1);
	}

	img->P56 = (char*)calloc(5, sizeof(char));
	if(!img) {
		exit(1);
	}
	fscanf(fin, "%s", img->P56);
	
	fscanf(fin, "%d", &img->width);
	fscanf(fin, "%d", &img->height);
	fscanf(fin, "%d\n", &img->maxval);
	
	unsigned int i;

	if(strcmp(img->P56, "P6") == 0) {

		img->grayscale_picture = NULL;
		
		img->color_picture = malloc(img->height * sizeof(color *));
		if(!img->color_picture) {
			exit(1);
		}

		for(i = 0; i < img->height; i++) {
			img->color_picture[i] = calloc(img->width, sizeof(color));
			if(!img->color_picture[i]) {
				exit(1);
			}
		}

		for(i = 0; i < img->height; i++) {
			fread(img->color_picture[i], sizeof(color), img->width, fin);
		}

	} else if(strcmp(img->P56, "P5") == 0){

		img->color_picture = NULL;
		
		img->grayscale_picture = malloc(img->height * sizeof(unsigned char *));
		if(!img->grayscale_picture) {
			exit(1);
		}
		
		for(i = 0; i < img->height; i++) {
			img->grayscale_picture[i] = calloc(img->width, sizeof(unsigned char));
			if(!img->grayscale_picture[i]) {
				exit(1);
			}
		}

		for(i = 0; i < img->height; i++) {
			fread(img->grayscale_picture[i], sizeof(unsigned char), img->width, fin);
		}

	} else {
		exit(1);
	}

	fclose(fin);
}

void writeData(const char * fileName, image *img) {

	FILE* fout = fopen(fileName, "w");
	if(!fout) {
		exit(1);
	}

	fprintf(fout, "%s\n", img->P56);
	fprintf(fout, "%d %d\n", img->width, img->height);
	fprintf(fout, "%d\n", img->maxval);
	
	unsigned int i;

	if(strcmp(img->P56, "P6") == 0) {

		for(i = 0; i < img->height; i++) {
			fwrite(img->color_picture[i], sizeof(color), img->width, fout);
		}

	} else if(strcmp(img->P56, "P5") == 0) {

		for(i = 0; i < img->height; i++) {
			fwrite(img->grayscale_picture[i], sizeof(unsigned char), img->width, fout);
		}

	} else {
		exit(1);
	}

	fclose(fout);

	if(strcmp(img->P56, "P6") == 0) {
		
		for(i = 0; i < img->height; i++) {
			free(img->color_picture[i]);
		}
		free(img->color_picture);

	} else if(strcmp(img->P56, "P5") == 0) {
		
		for(i = 0; i < img->height; i++) {
			free(img->grayscale_picture[i]);
		}
		free(img->grayscale_picture);
	}

	free(img->P56);
}

void* threadFunction(void *var)
{
	int thread_id = ((struct helper*)var)->thread_id;
	image *in = ((struct helper*)var)->in;
	image *out = ((struct helper*)var)->out;

	int N = resize_factor * out->height;
	int index = N / (num_threads * resize_factor);
	int start = thread_id * index * resize_factor;
	int end = start + index * resize_factor;

	if (thread_id == num_threads - 1) {
		end = N;
	}

	int i, j, i1, j1;
	int resize_factor2 = resize_factor * resize_factor;	
	const unsigned char g_kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
	const unsigned char g_kernel_sum = 16;


	if(strcmp(out->P56, "P6") == 0) {

		for(i = start; i < end; i += resize_factor) {
				for(j = 0; j < resize_factor * out->width; j += resize_factor) {
					
					if(resize_factor % 2 == 0) {

						unsigned int sum_r = 0, sum_g = 0, sum_b = 0;

						for(i1 = i; i1 < i + resize_factor; i1++) {
							for(j1 = j; j1 < j + resize_factor; j1++) {

								sum_r += (unsigned char)in->color_picture[i1][j1].r;
								sum_g += (unsigned char)in->color_picture[i1][j1].g;
								sum_b += (unsigned char)in->color_picture[i1][j1].b;
							}	
						}

						color *color_to_write = &out->color_picture[i / resize_factor][j / resize_factor];

						unsigned char ma_r = (unsigned char) (sum_r / resize_factor2);
						color_to_write->r = ma_r;	

						unsigned char ma_g= (unsigned char) (sum_g / resize_factor2);
						color_to_write->g = ma_g;

						unsigned char ma_b = (unsigned char) (sum_b / resize_factor2);
						color_to_write->b = ma_b;

					} else if(resize_factor == 3) {

						unsigned int sum_r = 0, sum_g = 0, sum_b = 0;

						for(i1 = i; i1 < i + resize_factor; i1++) {
							for(j1 = j; j1 < j + resize_factor; j1++) {

								sum_r += (unsigned int)(in->color_picture[i1][j1].r * g_kernel[i1 % 3][j1 % 3]);
								sum_g += (unsigned int)(in->color_picture[i1][j1].g * g_kernel[i1 % 3][j1 % 3]);
								sum_b += (unsigned int)(in->color_picture[i1][j1].b * g_kernel[i1 % 3][j1 % 3]);
							}	
						}

						color *color_to_write = &out->color_picture[i / resize_factor][j / resize_factor];

						unsigned char ma_r = (unsigned char) (sum_r / g_kernel_sum);
						color_to_write->r = ma_r;	

						unsigned char ma_g= (unsigned char) (sum_g / g_kernel_sum);
						color_to_write->g = ma_g;

						unsigned char ma_b = (unsigned char) (sum_b / g_kernel_sum);
						color_to_write->b = ma_b;

					}
				}
			}

		} else if(strcmp(out->P56, "P5") == 0) {

			for(i = start; i < end; i += resize_factor) {
				for(j = 0; j < resize_factor * out->width; j += resize_factor) {
					
					if(resize_factor % 2 == 0) {

						unsigned int sum = 0;

						for(i1 = i; i1 < i + resize_factor; i1++) {
							for(j1 = j; j1 < j + resize_factor; j1++) {
								sum += (unsigned int)in->grayscale_picture[i1][j1];
							}
						}

						unsigned char ma = (unsigned char) (sum / resize_factor2);
						out->grayscale_picture[i / resize_factor][j / resize_factor] = ma;

					} else if(resize_factor == 3) {

						unsigned int sum = 0;

						for(i1 = i; i1 < i + resize_factor; i1++) {
							for(j1 = j; j1 < j + resize_factor; j1++) {
								sum += (unsigned int)(in->grayscale_picture[i1][j1] * g_kernel[i1 % 3][j1 % 3]);
							}
						}

						unsigned char ma = (unsigned char) (sum / g_kernel_sum);					
						out->grayscale_picture[i / resize_factor][j / resize_factor] = ma;
					}						
				}
			}

		} else {
			exit(1);
		}

	return NULL;
}

void resize(image *in, image *out) { 

	out->P56 = (char*)calloc(5, sizeof(char));
	if(!out) {
		exit(1);
	}
	out->P56 = in->P56;
	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->maxval = in->maxval;

	int i;

	if(strcmp(out->P56, "P6") == 0) {

		out->color_picture = malloc(out->height * sizeof(color *));
		if(!out->color_picture) {
			exit(1);
		}

		for(i = 0; i < out->height; i++) {
			out->color_picture[i] = calloc(out->width, sizeof(color));
			if(!out->color_picture[i]) {
				exit(1);
			}
		}	

	} else if(strcmp(out->P56, "P5") == 0) {

		out->grayscale_picture = malloc(out->height * sizeof(unsigned char *));
		if(!out->grayscale_picture) {
			exit(1);
		}
		
		for(i = 0; i < out->height; i++) {
			out->grayscale_picture[i] = calloc(out->width, sizeof(unsigned char));
			if(!out->grayscale_picture[i]) {
				exit(1);
			}
		}		

	} else {
		exit(1);
	}

	struct helper *help;
	pthread_t tid[num_threads];

	for(i = 0; i < num_threads; i++) {

		help = (struct helper*) malloc(sizeof(struct helper));

		help->thread_id = i;
		help->in = in;
		help->out = out;

		pthread_create(&(tid[i]), NULL, threadFunction, (void*)help);
	}

	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}