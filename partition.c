#include <pgm.h>
#include<string.h>
#include <time.h>
#define HI(num)	(((num) & 0x0000FF00) >> 8)
#define LO(num)	((num) & 0x000000FF)
#define DATA_SET_SIZE 10
#define DATA_SET_SIZE_FOR_TRAINING 7
#define NUMBER_OF_PERSONS 40
#define PATH "orl_faces/" 
#define IMG_DIMENSION 10304
#define NN_OUTPUT_SIZE 40


typedef struct _PGMData {
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGMData;

int **allocate_dynamic_matrix(int row, int col)
{
	int **ret_val;
	int i;

	ret_val = (int **)malloc(sizeof(int *) * row);
	if (ret_val == NULL) {
		perror("memory allocation failure");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < row; ++i) {
		ret_val[i] = (int *)malloc(sizeof(int) * col);
		if (ret_val[i] == NULL) {
			perror("memory allocation failure");
			exit(EXIT_FAILURE);
		}
	}

	return ret_val;
}

void deallocate_dynamic_matrix(int **matrix, int row)
{
	int i;

	for (i = 0; i < row; ++i)
		free(matrix[i]);
	free(matrix);
}

void SkipComments(FILE *fp)
{
	int ch;
	char line[100];

	while ((ch = fgetc(fp)) != EOF && isspace(ch))
		;
	if (ch == '#') {
		fgets(line, sizeof(line), fp);
		SkipComments(fp);
	} else
		fseek(fp, -1, SEEK_CUR);
}


/*for reading:*/
void readPGM(char *file_name, PGMData *data)
{
	FILE *pgmFile;
	char version[3];
	int i, j;
	int lo, hi;

	pgmFile = fopen(file_name, "rb");
	if (pgmFile == NULL) {
		perror("cannot open file to read");
		exit(EXIT_FAILURE);
	}

	fgets(version, sizeof(version), pgmFile);
	if (strcmp(version, "P5")) {
		fprintf(stderr, "Wrong file type!\n");
		exit(EXIT_FAILURE);
	}

	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->col);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->row);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->max_gray);
	fgetc(pgmFile);

	data->matrix = allocate_dynamic_matrix(data->row, data->col);
	if (data->max_gray > 255){
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				hi = fgetc(pgmFile);
				lo = fgetc(pgmFile);
				data->matrix[i][j] = (hi << 8) + lo;
			}
	}
	else{
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				lo = fgetc(pgmFile);
				//printf("%d\n",lo);
				data->matrix[i][j] = lo;
			}
    }
	fclose(pgmFile);

}

/*and for writing*/

void writePGM(const char *filename, const PGMData *data)
{
	FILE *pgmFile;
	int i, j;
	int hi, lo;

	pgmFile = fopen(filename, "wb");
	if (pgmFile == NULL) {
		perror("cannot open file to write");
		exit(EXIT_FAILURE);
	}

	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", data->col, data->row);
	fprintf(pgmFile, "%d ", data->max_gray);

	if (data->max_gray > 255) {
		for (i = 0; i < data->row; ++i) {
			for (j = 0; j < data->col; ++j) {
				hi = HI(data->matrix[i][j]);
				lo = LO(data->matrix[i][j]);
				fputc(hi, pgmFile);
				fputc(lo, pgmFile);
			}

		}
	} else {
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				lo = LO(data->matrix[i][j]);
				fputc(lo, pgmFile);
			}
	}

	fclose(pgmFile);
	deallocate_dynamic_matrix(data->matrix, data->row);
}
void PGM_file_write(FILE *file, char* path){
    int i, j, pixel=0;
    //double ave=0.0;
    PGMData *pic = malloc(sizeof(PGMData));
    readPGM(path, pic);
    //printf("%d\n",pic.row);
    //printf("%d\n",pic.col);
    //fprintf(file,"%i ", (int)ave);
    for (i=0;i<pic->row;i++){
        for(j=0;j<pic->col;j++){
                pixel = (int) pic->matrix[i][j];
                //printf("%i ", pixel);
                fprintf(file,"%i ",pixel);
        }
    }
    fprintf(file,"\n");
}

/* returns a random integer array bounded by the range specified */
int* randomize_int_array(int* random_array,int from, int to) {
    // make this more random?
    int i, temp, r, counter=0, size=to-from+1;
    for(i=from; i<=to; i++){
        random_array[counter] = i;
        counter++;
    }

    for (i = 0; i < size; i++) {
        r = rand() % size;
        temp = random_array[i];
        random_array[i] = random_array[r];
        random_array[r] = temp;
     }
     /*
    for(i=0; i<size; i++){
        printf("%d ", random_array[i]);
    }
    printf("\n");
    */
    return random_array;
}

void disjoin_sets(FILE *train, FILE *test, int whole_size, int train_size, char* input_path){
    int i, j, k;
    char filename[35];
    srand ( (unsigned)time ( NULL ) );
    for(i=1; i<=NUMBER_OF_PERSONS; i++){
        int* rand_int_array = (int *)malloc(sizeof(int)*whole_size);
        randomize_int_array(rand_int_array, 1, whole_size);
        for(j=0; j<whole_size-train_size;j++){
            sprintf(filename, "%s%s%d%s%d%s", input_path, "s", i, "/", rand_int_array[j], ".pgm");
            //printf("%s\n", filename);
            PGM_file_write(test, filename);
            
        }
        for(j=whole_size-train_size; j < whole_size;j++){
            sprintf(filename, "%s%s%d%s%d%s", input_path, "s", i, "/", rand_int_array[j], ".pgm");
            //printf("%s\n", filename);
            PGM_file_write(train, filename);
            //fprintf(train, "%d\n", i);
            
            for(k=1; k<=NUMBER_OF_PERSONS; k++){
                fprintf(train,"%d ", k==i);
            }
            
            fprintf(train, "\n");
        }
    }    
}

int main(){
    FILE *train;
    train = fopen("train.data","w+"); 
    fprintf(train,"%d %d %d\n", DATA_SET_SIZE_FOR_TRAINING*NUMBER_OF_PERSONS, IMG_DIMENSION, NN_OUTPUT_SIZE); // TODO: change out value
    
    FILE *test;
    test = fopen("test.data","w+"); 
    fprintf(test,"%d %d\n", DATA_SET_SIZE-DATA_SET_SIZE_FOR_TRAINING, IMG_DIMENSION);
    
    disjoin_sets(train, test, DATA_SET_SIZE, DATA_SET_SIZE_FOR_TRAINING, PATH);
    fclose(test);
    fclose(train);
}
