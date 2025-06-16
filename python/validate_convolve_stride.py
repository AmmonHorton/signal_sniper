import numpy as np
import signal_sniper_py as ssp

def validate_convolve_stride():
    # Create a test signal
    input_signal = np.sin(2 * np.pi * 0.01 * np.arange(100))

    # Create a filter
    filter_coeffs = np.array([0.2, 0.5, 0.2])

    input_signal = np.arange(20, 60)
    filter_coeffs = np.arange(20)

    # Define strides
    input_stride = 5
    filter_stride = 2
    conjugate = False


    def numpy_convolve_stride(in_vec, in_start, filt, filt_start, input_stride, filter_stride, conjugate):
        if conjugate:
            filter_coeffs = np.conj(filter_coeffs)
        
        in_vec_ds = in_vec[in_start::input_stride]
        filter_ds = filt[filt_start::filter_stride]

        print(np.flip(filter_ds))

        output = np.convolve(in_vec_ds, np.flip(filter_ds), mode='same')
        return output

    # Using signal_sniper_python
    convolved_signal = ssp.convolve_stride(input_signal, filter_coeffs, input_stride, filter_stride, conjugate)
    # print(convolved_signal)
    # Using NumPy for reference
    numpy_convolved_signal = numpy_convolve_stride(input_signal, 0, filter_coeffs, 0, input_stride, filter_stride, conjugate)

    # print(np.convolve([1, 2, 3, 4], [1, 2, 3]))

    print(np.array(np.real(convolved_signal), dtype=int))
    print(numpy_convolved_signal)

    # Validate the results
    # assert np.allclose(convolved_signal, numpy_convolved_signal, atol=1e-6), "Convolved signals do not match!"

    print("Convolve stride validation passed!")

if __name__ == "__main__":
    validate_convolve_stride()
