import numpy as np
import signal_sniper_py as ssp

def validate_fft():
    input_array = np.array([1+1j, 2+2j, 3+3j, 4+4j], dtype=np.complex128)

    # Using NumPy for reference
    numpy_fft1d = np.fft.fft(input_array)
    numpy_ifft1d = np.fft.ifft(input_array, norm='forward')

    # Using signal_sniper_python
    fft1d_result = np.array(ssp.fft1d(input_array, inverse=False))  # FFTW_FORWARD
    ifft1d_result = np.array(ssp.fft1d(input_array, inverse=True))  # FFTW_BACKWARD

    print("NumPy FFT1D Result: ", numpy_fft1d)
    print("Signal Sniper FFT1D Result: ", fft1d_result)
    print("NumPy IFFT1D Result: ", numpy_ifft1d)
    print("Signal Sniper IFFT1D Result: ", ifft1d_result)

    assert np.allclose(numpy_fft1d, fft1d_result), "FFT1D results do not match!"
    assert np.allclose(numpy_ifft1d, ifft1d_result), "IFFT1D results do not match!"


    return
    input_array_2d = np.array([[1+1j, 2+2j], [3+3j, 4+4j]], dtype=np.complex128)

    # Using NumPy for reference
    numpy_fft2d = np.fft.fft2(input_array_2d)
    numpy_ifft2d = np.fft.ifft2(input_array_2d, norm='forward')

    # Using signal_sniper_python
    fft2d_result = ssp.fft2d(np.reshape(input_array_2d, (1,-1))[0], inverse=False, nx=2, ny=2)  # FFTW_FORWARD
    ifft2d_result = ssp.fft2d(np.reshape(input_array_2d, (1,-1))[0], inverse=True, nx=2, ny=2)  # FFTW_BACKWARD

    print("NumPy FFT2D Result: ", numpy_fft2d)
    print("Signal Sniper FFT2D Result: ", fft2d_result)
    print("NumPy IFFT2D Result: ", numpy_ifft2d)
    print("Signal Sniper IFFT2D Result: ", ifft2d_result)

    assert np.allclose(numpy_fft2d, fft2d_result), "FFT2D results do not match!"
    assert np.allclose(numpy_ifft2d, ifft2d_result), "IFFT2D results do not match!"

    # Validate FFT1D class
    fft1d_instance = ssp.FFT1D(4)
    fft1d_class_result = fft1d_instance.execute(input_array, inverse=False)  # FFTW_FORWARD
    ifft1d_class_result = fft1d_instance.execute(input_array, inverse=True)  # FFTW_BACKWARD

    print("Signal Sniper FFT1D Class Result: ", fft1d_class_result)
    print("Signal Sniper IFFT1D Class Result: ", ifft1d_class_result)

    assert np.allclose(numpy_fft1d, fft1d_class_result), "FFT1D class results do not match!"
    assert np.allclose(numpy_ifft1d, ifft1d_class_result), "IFFT1D class results do not match!"

if __name__ == "__main__":
    validate_fft()