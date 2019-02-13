#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	unsigned char r, g, b;
}color;

typedef struct {
	char *P56;
	int width, height, maxval;
	color **color_picture;
	unsigned char **grayscale_picture;
}image;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image *out);

#endif /* HOMEWORK_H */