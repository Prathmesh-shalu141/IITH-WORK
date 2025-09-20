#include <adf.h>
#include "kernels.h"
#include "kernels/include.h"

using namespace adf;

class simpleGraph : public adf::graph {
private:
  kernel k1, k2, k3, k4, k5, sum_kernel;
  public:
  input_plio in1, in2, in3, in4, in5;
  output_plio out;

  simpleGraph(){
    // Create input and output PLIOs
    in1 = input_plio::create(plio_32_bits, "data/input1.txt");
    in2 = input_plio::create(plio_32_bits, "data/input2.txt");
    in3 = input_plio::create(plio_32_bits, "data/input3.txt");
    in4 = input_plio::create(plio_32_bits, "data/input4.txt");
    in5 = input_plio::create(plio_32_bits, "data/input5.txt");
    out = output_plio::create(plio_32_bits, "data/output.txt");

    // Create the kernels
    k1 = kernel::create(kernel1);
    k2 = kernel::create(kernel2);
    k3 = kernel::create(kernel3);
    k4 = kernel::create(kernel4);
    k5 = kernel::create(kernel5);
    sum_kernel = kernel::create(sum_kernel_function);


    // Connect kernels to ai engine tiles
    location<kernel>(k1) = tile(8,0);
	location<kernel>(k2) = tile(8,1);
    location<kernel>(k3) = tile(8,2);
    location<kernel>(k4) = tile(8,3);
    location<kernel>(k5) = tile(8,4);
    location<kernel>(sum_kernel) = tile(8,5);

    // Connect input PLIOs to the kernels
    adf::connect(in1.out[0], k1.in[0]);
    adf::connect(in2.out[0], k2.in[0]);
    adf::connect(in3.out[0], k3.in[0]);
    adf::connect(in4.out[0], k4.in[0]);
    adf::connect(in5.out[0], k5.in[0]);

    // Connect kernels to the sum kernel
    adf::connect(k1.out[0], sum_kernel.in[0]);
    adf::connect(k2.out[0], sum_kernel.in[1]);
    adf::connect(k3.out[0], sum_kernel.in[2]);
    adf::connect(k4.out[0], sum_kernel.in[3]);
    adf::connect(k5.out[0], sum_kernel.in[4]);

    // Connect the sum kernel to the output PLIO
    adf::connect(sum_kernel.out[0], out.in[0]);

    // Set the source files for the kernels
    source(k1) = "kernels/kernels.cc";
    source(k2) = "kernels/kernels.cc";
    source(k3) = "kernels/kernels.cc";
    source(k4) = "kernels/kernels.cc";
    source(k5) = "kernels/kernels.cc";
    source(sum_kernel) = "kernels/kernels.cc";

    // Set the runtime ratios for the kernels
    runtime<ratio>(k1) = 0.1;
    runtime<ratio>(k2) = 0.1;
    runtime<ratio>(k3) = 0.1;
    runtime<ratio>(k4) = 0.1;
    runtime<ratio>(k5) = 0.1;
    runtime<ratio>(sum_kernel) = 0.1;

    // Define dimensions for input and output buffers
    dimensions(k1.in[0]) = { 100 };  // Input size of 100 integers for each kernel
    dimensions(k2.in[0]) = { 100 };
    dimensions(k3.in[0]) = { 100 };
    dimensions(k4.in[0]) = { 100 };
    dimensions(k5.in[0]) = { 100 };
    dimensions(k1.out[0]) = { 1 };   // Output dimension for each kernel, storing a single integer (partial sum)
    dimensions(k2.out[0]) = { 1 };
    dimensions(k3.out[0]) = { 1 };
    dimensions(k4.out[0]) = { 1 };
    dimensions(k5.out[0]) = { 1 };
    dimensions(sum_kernel.in[0]) = { 1 }; // Input dimension for sum kernel (partial sum from each kernel)
    dimensions(sum_kernel.in[1]) = { 1 };
    dimensions(sum_kernel.in[2]) = { 1 };
    dimensions(sum_kernel.in[3]) = { 1 };
    dimensions(sum_kernel.in[4]) = { 1 };
    dimensions(sum_kernel.out[0]) = { 1 }; // Output dimension for sum kernel, storing a single integer (final sum)
  }
};
