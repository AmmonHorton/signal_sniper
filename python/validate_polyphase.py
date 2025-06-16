import numpy as np
import signal_sniper_py as ssp

def validate_polyphase():
    # Create a test signal
    input_signal = np.sin(2 * np.pi * 0.01 * np.arange(104))

    # Create a low-pass filter
    num_taps = 32
    cutoff = 0.1
    filter_coeffs = np.sinc(2 * cutoff * (np.arange(num_taps) - (num_taps - 1) / 2))
    filter_coeffs *= np.hamming(num_taps)
    filter_coeffs /= np.sum(filter_coeffs)

    # input_signal = np.arange(100)
    # filter_coeffs = np.arange(20)


    # Polyphase interpolation
    factor = 4
    polyphase_interpolator = ssp.Polyphase(factor, filter_coeffs)
    
    
    np.pad(filter_coeffs, (0, int(np.ceil(num_taps/factor) * factor)), 'constant')
    filter_np = np.array([filter_coeffs[i::factor] for i in range(factor)])
    # filter_branches_flat = polyphase_interpolator.get_filter_slices()
    # filter_branches = np.reshape(filter_branches_flat, (factor,len(filter_branches_flat)//factor))
    # print(filter_np)
    # print(filter_branches)
    
    
    # Break into branches
    branches_flat = polyphase_interpolator.branch(input_signal)
    branches = np.reshape(branches_flat, (factor,len(input_signal)//factor))
    branches_np = np.array([input_signal[i::factor] for i in range(factor)])
    # print(branches_np)
    # print(np.round(branches))
    assert np.allclose(branches, branches_np), "Branches do not match!"

    # Convolve branches
    convolved_branches_flat = polyphase_interpolator.convolve_branches_interpolate(input_signal)
    convolved_branches = np.reshape(convolved_branches_flat, (factor,len(input_signal)))
    convolved_branches_np = np.array([np.convolve(input_signal, filter_np[i], mode='same') for i in range(factor)])
    # print(convolved_branches_np)
    # print(np.array(np.round(convolved_branches), dtype=int))
    assert np.allclose(convolved_branches, convolved_branches_np), "Convolved branches do not match!"

    # Interleave branches
    interleaved_branches = polyphase_interpolator.interleave(convolved_branches_flat)
    interleaved_branches_np = np.zeros_like(convolved_branches_np.flatten())
    for i in range(factor):
        interleaved_branches_np[i::factor] = convolved_branches_np[i]
    assert np.allclose(interleaved_branches, interleaved_branches_np), "Interleaved branches do not match!"

    # Polyphase decimation
    polyphase_decimator = ssp.Polyphase(factor, num_taps)
    decimated_signal = polyphase_decimator.decimate(input_signal)
    filtered_signal = np.convolve(input_signal, filter_coeffs, mode='same')
    numpy_decimated_signal = filtered_signal[::factor]
    assert np.allclose(decimated_signal, numpy_decimated_signal, atol=1e-6), "Decimation results do not match!"


    # Interpolation
    interpolated_signal = polyphase_interpolator.interpolate(input_signal)
    upsampled_signal = np.zeros(len(input_signal) * factor)
    upsampled_signal[::factor] = input_signal
    numpy_interpolated_signal = np.convolve(upsampled_signal, filter_coeffs, mode='same')
    assert np.allclose(interpolated_signal, numpy_interpolated_signal, atol=1e-6), "Interpolation results do not match!"


    print("Polyphase interpolation and decimation validation passed!")

if __name__ == "__main__":
    validate_polyphase()