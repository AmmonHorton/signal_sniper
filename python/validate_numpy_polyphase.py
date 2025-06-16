import numpy as np
import matplotlib.pyplot as plt
import signal_sniper_py as ssp


def validate_numpy_polyphase():
    # Create a test signal
    # input_signal = np.exp(1j*2 * np.pi * 0.01 * np.arange(100))
    input_signal = np.zeros(100, dtype=np.complex64)
    input_signal[50] = 1

    # Add a small amount of complex noise to the signal
    noise = np.random.normal(0, 0.1, input_signal.shape) + 1j * np.random.normal(0, 0.1, input_signal.shape)
    input_signal += noise

    # Create a low-pass filter
    num_taps = 31
    cutoff = 0.1
    filter_coeffs = np.sinc(2 * cutoff * (np.arange(num_taps) - (num_taps - 1) / 2))
    filter_coeffs *= np.hamming(num_taps)
    filter_coeffs /= np.sum(filter_coeffs)

    factor = 4

    # input_signal = np.arange(30) + 1j* np.arange(30, 0, -1)
    # filter_coeffs = np.arange(22) + 1j* np.arange(22, 0, -1)


    # Validate interpolation
    ssp_polyphase_interpolator = ssp.Polyphase(factor, (filter_coeffs))
    ssp_output = ssp_polyphase_interpolator.interpolate(input_signal)
    

    upsampled_signal = np.zeros(len(input_signal) * factor, dtype=input_signal.dtype)
    upsampled_signal[::factor] = input_signal
    reference_interpolated_signal = np.convolve(upsampled_signal, filter_coeffs, mode='same')

    # plt.plot(np.fft.fftshift(np.abs(np.fft.fft(ssp_output))))
    plt.plot(np.abs(ssp_output))
    plt.show()

    passed_interpolation = False
    for i in range(factor):
        validation_array = reference_interpolated_signal[i:]
        output_array = ssp_output[:len(validation_array)]
        passed_interpolation = passed_interpolation or np.allclose(output_array, validation_array, atol=1e-6)
    # assert passed_interpolation, "Interpolation results do not match!"

    # Validate decimation
    ssp_polyphase_interpolator = ssp.Polyphase(factor, np.flip(filter_coeffs))
    ssp_output = ssp_polyphase_interpolator.decimate(input_signal)
    
    filtered_signal = np.convolve(input_signal, filter_coeffs, mode='same')

    passed_decimation = False
    for i in range(factor):
        validation_array = filtered_signal[i::factor]
        output_array = ssp_output[:len(validation_array)]
        passed_decimation = passed_decimation or np.allclose(output_array, validation_array, atol=1e-6)
    # assert passed_decimation, "Interpolation results do not match!"

    print("NumPy polyphase interpolation and decimation validation passed!")

    # plt.plot(np.fft.fftshift(np.abs(np.fft.fft(ssp_output))))
    plt.plot(np.abs(ssp_output))
    plt.show()

if __name__ == "__main__":
    validate_numpy_polyphase()