#include <adf.h>
#include <stdio.h>  // Include for printf

void kernel1(adf::input_buffer<int> &in, adf::output_buffer<int> &out){
    int sum = 0;
    int* inItr = in.data();
    int* outItr = out.data();
    for (int i = 0; i < 100; i++) {
        sum += *inItr++;
    }
    *outItr = sum;
}
void kernel2(adf::input_buffer<int> &in, adf::output_buffer<int> &out){
int sum = 0;
int* inItr = in.data();
int* outItr = out.data();
for (int i = 0; i < 100; i++) {
sum += *inItr++;
}
*outItr = sum;
}

void kernel3(adf::input_buffer<int> &in, adf::output_buffer<int> &out){
int sum = 0;
int* inItr = in.data();
int* outItr = out.data();
for (int i = 0; i < 100; i++) {
sum += *inItr++;
}
*outItr = sum;
}

void kernel4(adf::input_buffer<int> &in, adf::output_buffer<int> &out){
int sum = 0;
int* inItr = in.data();
int* outItr = out.data();
for (int i = 0; i < 100; i++) {
sum += *inItr++;
}
*outItr = sum;
}

void kernel5(adf::input_buffer<int> &in, adf::output_buffer<int> &out){
int sum = 0;
int* inItr = in.data();
int* outItr = out.data();
for (int i = 0; i < 100; i++) {
sum += *inItr++;
}
*outItr = sum;
}
    
void sum_kernel_function(adf::input_buffer<int> &in1, adf::input_buffer<int> &in2, adf::input_buffer<int> &in3, adf::input_buffer<int> &in4, adf::input_buffer<int> &in5, adf::output_buffer<int> &out){
    int sum1 = *in1.data();
    int sum2 = *in2.data();
    int sum3 = *in3.data();
    int sum4 = *in4.data();
    int sum5 = *in5.data();
    int final_sum = sum1 + sum2 + sum3 + sum4 + sum5;
    *out.data() = final_sum;
}
