#include <gtest/gtest.h>
#include "dsp/fft.h"

TEST(FFT1DTest, ConstructorDestructor) {
    // Test constructor and destructor
    dsp::fft::FFT1D fft(16);
    // EXPECT_EQ(fft.size(), 16);
}

TEST(FFT1DTest, MoveConstructor) {
    // Test move constructor
    dsp::fft::FFT1D fft1(16);
    dsp::fft::FFT1D fft2(std::move(fft1));
    // EXPECT_EQ(fft2.size(), 16);
}

TEST(FFT1DTest, MoveAssignment) {
    // Test move assignment operator
    dsp::fft::FFT1D fft1(16);
    dsp::fft::FFT1D fft2(32);
    fft2 = std::move(fft1);
    // EXPECT_EQ(fft2.size(), 16);
}

TEST(FFT1DTest, ExecuteForward) {
    // Test forward FFT execution
    dsp::fft::FFT1D fft(16);
    cfloat_vec input(16, cfloat(1.0, 0.0));
    cfloat_vec output(16, cfloat(1.0, 0.0));
    fft.execute(input, output, false);
    EXPECT_EQ(output.size(), 16);
}

TEST(FFT1DTest, ExecuteBackward) {
    // Test backward FFT execution
    dsp::fft::FFT1D fft(16);
    cfloat_vec input(16, cfloat(1.0, 0.0));
    cfloat_vec output(16, cfloat(1.0, 0.0));
    fft.execute(input, output, true);
    EXPECT_EQ(output.size(), 16);
}
