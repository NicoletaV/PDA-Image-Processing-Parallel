#include "homework1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

int num_threads;
int resolution;

struct helper {
    unsigned char **grayscale_picture;
    int thread_id;
};

void initialize(image *im) {

	im->P56 = "P5";
	im->maxval = (unsigned char)255;

	im->grayscale_picture = malloc(resolution * sizeof(unsigned char *));
	if(!im->grayscale_picture) {
		exit(1);
	}
	
	for(unsigned int i = 0; i < resolution; i++) {
		im->grayscale_picture[i] = calloc(resolution, sizeof(unsigned char));
		if(!im->grayscale_picture[i]) {
			exit(1);
		}
	}
}

void* threadFunction(void *var)
{
	int thread_id = ((struct helper*)var)->thread_id;
	unsigned char **grayscale_picture = ((struct helper*)var)->grayscale_picture;

	int start = thread_id * resolution / num_threads;
	int end = start + resolution / num_threads;

	if (thread_id == num_threads - 1) {
		end = resolution;
	}

	double sqrt5 = sqrt(5.0f);
	double step = (double) 100 / resolution;

	for(int i = start; i < end; i++) {
		for(int j = 0; j < resolution; j++) {

			double y = step / 2 + (resolution - 1 - i) * step;
			double x = step / 2 + j * step;

			double distance = (abs(-x + 2 * y) / sqrt5);

			if(distance <= 3.0f) {
				grayscale_picture[i][j] = (unsigned char) 0;
			}
		}
	}

	return NULL;
}

void render(image *im) {

	int i;
	struct helper *help;
	pthread_t tid[num_threads];

	for(int i = 0; i < resolution; i++) {
		for(int j = 0; j < resolution; j++) {
			im->grayscale_picture[i][j] = (unsigned char) im->maxval;
		}
	}

	for(i = 0; i < num_threads; i++) {

		help = (struct helper*) malloc(sizeof(struct helper));
		help->grayscale_picture = im->grayscale_picture;
		help->thread_id = i;

		pthread_create(&(tid[i]), NULL, threadFunction, (void*)help);
	}

	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}

void writeData(const char * fileName, image *img) {

	FILE* fout = fopen(fileName, "w");
	if(!fout) {
		exit(1);
	}

	fprintf(fout, "%s\n", img->P56);
	fprintf(fout, "%d %d\n", resolution, resolution);
	fprintf(fout, "%d\n", img->maxval);
	
	unsigned int i;

	if(strcmp(img->P56, "P5") == 0) {

		for(i = 0; i < resolution; i++) {
			fwrite(img->grayscale_picture[i], sizeof(unsigned char), resolution, fout);
		}

	} else {
		exit(1);
	}

	fclose(fout);

	for(i = 0; i < resolution; i++) {
		free(img->grayscale_picture[i]);
	}
	
	free(img->grayscale_picture);	
}


