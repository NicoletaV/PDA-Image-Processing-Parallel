#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
	char *P56;
	unsigned char maxval, **grayscale_picture;
}image;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */