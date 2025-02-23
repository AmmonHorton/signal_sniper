import numpy as np
import signal_sniper_python as ssp

def validate_convolve():
    filter_sizes = [4, 5, 6, 7, 8, 9, 10, 50, 567]  # Even and odd filter sizes
    overlap_save_options = [True, False]

    epsilon = 1e-10

    for filter_size in filter_sizes:
        block_size = filter_size * 8
        input_sizes = [block_size - 1, block_size, block_size + 1, block_size * 10]  # Less than, equal to, and greater than one block size

        for input_size in input_sizes:
            for overlap_save in overlap_save_options:
                print(f"Testing with filter_size={filter_size}, input_size={input_size}, overlap_save={overlap_save}")

                # Generate random complex input and filter arrays
                input_array = np.random.rand(input_size) + 1j * np.random.rand(input_size)
                filter_array = np.random.rand(filter_size) + 1j * np.random.rand(filter_size)

                # Using NumPy for reference
                numpy_convolve_same = np.convolve(input_array, filter_array, mode='same')
                numpy_convolve_valid = np.convolve(input_array, filter_array, mode='valid')
                numpy_correlate_valid = np.correlate(input_array, filter_array, mode='valid')

                # Using signal_sniper_python
                convolve_result_same = ssp.convolve(input_array, filter_array, propogate_delay=True, fft_overlap_save=overlap_save)
                convolve_result_valid = ssp.convolve(input_array, filter_array, propogate_delay=False, fft_overlap_save=overlap_save)
                correlate_result_valid = ssp.correlate(input_array, filter_array, fft_overlap_save=overlap_save)

                # print("NumPy Convolve Same Result: ", numpy_convolve_same)
                # print("Signal Sniper Convolve Same Result: ", convolve_result_same)
                # print("NumPy Convolve Valid Result: ", numpy_convolve_valid)
                # print("Signal Sniper Convolve Valid Result: ", convolve_result_valid)
                # print("NumPy Correlate Valid Result: ", numpy_correlate_valid)
                # print("Signal Sniper Correlate Valid Result: ", correlate_result_valid)

                assert np.allclose(numpy_convolve_same, convolve_result_same), "Convolve same results do not match!"
                assert np.allclose(numpy_convolve_valid, convolve_result_valid), "Convolve valid results do not match!"
                assert np.allclose(numpy_correlate_valid, correlate_result_valid), "Correlate valid results do not match!"

if __name__ == "__main__":
    validate_convolve()