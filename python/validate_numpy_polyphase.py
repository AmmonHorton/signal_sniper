import numpy as np
import matplotlib.pyplot as plt
import signal_sniper_python as ssp

def validate_numpy_polyphase():
    # Create a test signal
    input_signal = np.exp(1j*2 * np.pi * 0.01 * np.arange(100))

    # Add a small amount of complex noise to the signal
    noise = np.random.normal(0, 0.1, input_signal.shape) + 1j * np.random.normal(0, 0.1, input_signal.shape)
    input_signal += noise

    # Create a low-pass filter
    num_taps = 32
    cutoff = 0.1
    filter_coeffs = np.sinc(2 * cutoff * (np.arange(num_taps) - (num_taps - 1) / 2))
    filter_coeffs *= np.hamming(num_taps)
    filter_coeffs /= np.sum(filter_coeffs)

    factor = 4

    input_signal = np.pad(np.arange(100), (20, 20), 'constant')
    filter_coeffs = np.arange(20)

    # Polyphase interpolation using NumPy
    def numpy_polyphase_interpolate(input_signal, filter_coeffs, factor):
        convolutions = np.array([np.convolve(input_signal, (filter_coeffs[i::factor]), mode='valid') for i in range(factor)])
        interleaved = np.reshape(convolutions.T, (-1))
        return interleaved

    # Polyphase decimation using NumPy
    def numpy_polyphase_decimate(input_signal, filter_coeffs, factor):
        convolutions = np.array([np.convolve(input_signal[i::factor], np.flip(filter_coeffs[i::factor]), mode='valid') for i in range(factor)])
        summed = np.sum(convolutions, axis=0)
        return summed

    # Validate interpolation
    ssp_polyphase_interpolator = ssp.Polyphase(factor, (filter_coeffs))
    ssp_output = ssp_polyphase_interpolator.interpolate(input_signal)
    numpy_interpolated_signal = numpy_polyphase_interpolate(input_signal, (filter_coeffs), factor)
    upsampled_signal = np.zeros(len(input_signal) * factor)
    upsampled_signal[::factor] = input_signal
    reference_interpolated_signal = np.convolve(upsampled_signal, filter_coeffs, mode='same')

    print(np.array(np.real(ssp_output), dtype=int))
    # print(np.array(np.real(numpy_interpolated_signal), dtype=int))
    print(np.array(np.real(reference_interpolated_signal), dtype=int))

    # ssp_output = np.reshape(ssp_polyphase_interpolator.decimate(input_signal), (4, -1))
    ssp_polyphase_interpolator = ssp.Polyphase(factor, np.flip(filter_coeffs))
    ssp_output = ssp_polyphase_interpolator.decimate(input_signal)
    numpy_decimated_signal = numpy_polyphase_decimate(input_signal, np.flip(filter_coeffs), factor)

    filtered_signal = np.convolve(input_signal, filter_coeffs, mode='same')
    reference_decimated_signal = filtered_signal[2::factor]

    # print(np.array(np.real(ssp_output), dtype=int))
    print(np.array(np.real(ssp_output), dtype=int))
    # print(numpy_decimated_signal)
    print(reference_decimated_signal)

    # # Take FFT of the reference and numpy polyphase interpolated signals
    # fft_reference_interpolated = np.fft.fft(reference_interpolated_signal)
    # fft_numpy_interpolated = np.fft.fft(numpy_interpolated_signal)

    # # Plot the FFTs
    # plt.figure()
    # plt.plot(np.abs(fft_reference_interpolated), label='Reference Interpolated FFT')
    # plt.plot(np.abs(fft_numpy_interpolated), label='NumPy Interpolated FFT', linestyle='--')
    # plt.legend()
    # plt.title('FFT of Interpolated Signals')
    # plt.xlabel('Frequency Bin')
    # plt.ylabel('Magnitude')
    # plt.show()

    assert np.allclose(numpy_interpolated_signal, reference_interpolated_signal, atol=1e-6), "Interpolation results do not match!"

    # Validate decimation

    assert np.allclose(numpy_decimated_signal, reference_decimated_signal, atol=1e-6), "Decimation results do not match!"

    print("NumPy polyphase interpolation and decimation validation passed!")

if __name__ == "__main__":
    validate_numpy_polyphase()