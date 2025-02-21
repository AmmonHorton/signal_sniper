import numpy as np
import signal_sniper_python as ssp

def validate_convolve():
    input_array = np.array([1+1j, 2+2j, 3+3j, 4+4j], dtype=np.complex128)
    filter_array = np.array([1+1j, 1+1j], dtype=np.complex128)

    # Using NumPy for reference
    numpy_convolve = np.convolve(input_array, filter_array, mode='full')
    numpy_correlate = np.correlate(input_array, filter_array, mode='full')

    # Using signal_sniper_python
    convolve_result = ssp.convolve(input_array, filter_array, propogate_delay=True, fft_overlap_save=False)
    correlate_result = ssp.correlate(input_array, filter_array, fft_overlap_save=False)

    print("NumPy Convolve Result: ", numpy_convolve)
    print("Signal Sniper Convolve Result: ", convolve_result)
    print("NumPy Correlate Result: ", numpy_correlate)
    print("Signal Sniper Correlate Result: ", correlate_result)

    assert np.allclose(numpy_convolve, convolve_result), "Convolve results do not match!"
    assert np.allclose(numpy_correlate, correlate_result), "Correlate results do not match!"

if __name__ == "__main__":
    validate_convolve()