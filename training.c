/*http://ugurkoltuk.wordpress.com/2010/03/04/an-extreme-simple-pgm-io-api/ */

#include "fann.h"
#include "floatfann.h"

int main(){
    const unsigned int num_input = 10304;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 1000;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 50000;
    const unsigned int epochs_between_reports = 10;
    
    struct fann *ann = fann_create_standard(num_layers, num_input,
        num_neurons_hidden, num_output);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL);
    fann_train_on_file(ann, "train.data", max_epochs,
        epochs_between_reports, desired_error);
    fann_save(ann, "ann.net");
    fann_destroy(ann);
    
    /*
    fann_type *calc_out;
    fann_type input[2];
    //struct fann *
    ann = fann_create_from_file("xor_float.net");
    input[0] = -1;
    input[1] = 1;
    calc_out = fann_run(ann, input);
    printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
    fann_destroy(ann); 
    */   
}


