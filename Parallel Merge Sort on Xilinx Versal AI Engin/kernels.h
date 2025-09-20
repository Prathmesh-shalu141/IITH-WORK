
#ifndef FUNCTION_KERNELS_H
#define FUNCTION_KERNELS_H

 void kernel1(adf::input_buffer<int> &in, adf::output_buffer<int> &out);
 void kernel2(adf::input_buffer<int> &in, adf::output_buffer<int> &out);
 void kernel3(adf::input_buffer<int> &in, adf::output_buffer<int> &out);
 void kernel4(adf::input_buffer<int> &in, adf::output_buffer<int> &out);
void kernel5(adf::input_buffer<int> &in, adf::output_buffer<int> &out);
void sum_kernel_function(adf::input_buffer<int> &in1, adf::input_buffer<int> &in2, adf::input_buffer<int> &in3, adf::input_buffer<int> &in4, adf::input_buffer<int> &in5, adf::output_buffer<int> &out); 

#endif
