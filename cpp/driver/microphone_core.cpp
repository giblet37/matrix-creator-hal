#include <valarray>

#include "cpp/driver/creator_memory_map.h"
#include "cpp/driver/fir_coeff.h"
#include "cpp/driver/microphone_core.h"
#include "cpp/driver/microphone_core_fir.h"


namespace matrix_hal {

MicrophoneCore::MicrophoneCore(MicrophoneArray &mics) : mics_(mics) {
  fir_coeff_.resize(kNumberFIRTaps);
}

MicrophoneCore::~MicrophoneCore() {}

void MicrophoneCore::Setup(MatrixIOBus *bus) {
  MatrixDriver::Setup(bus);
  SetCustomFIRCoeff(&FIR_default[0]);
}

bool MicrophoneCore::SetFIRCoeff() {
  return bus_->Write(kMicrophoneCoreBaseAddress,
                     reinterpret_cast<unsigned char *>(&fir_coeff_[0]),
                     fir_coeff_.size());
}

bool MicrophoneCore::SetCustomFIRCoeff(
    const std::valarray<int16_t> custom_fir) {
  if (custom_fir.size() == kNumberFIRTaps) {
    fir_coeff_ = custom_fir;
    return SetFIRCoeff();
  } else {
    std::cout << "Size FIR Filter must be : " << kNumberFIRTaps << std::endl;
    return false;
  }
}

bool MicrophoneCore::SetCustomFIRCoeff(FIRCoeff *FIR_coeff) {
  SelectFIRCoeff(FIR_coeff, mics_.SamplingRate());
}

bool MicrophoneCore::SelectFIRCoeff(FIRCoeff *FIR_coeff,
                                    uint32_t sampling_frequency) {
  if (sampling_frequency == 0) {
    std::cerr << "Bad Configuration, sampling_frequency must be greather than 0"
              << std::endl;
    return false;
  }

  for (int i = 0;; i++) {
    if (FIR_coeff[i].rate_ == 0) {
      std::cerr << "Unsoported sampling frequency, it must be: 8000, 12000, "
                   "16000, 22050, 24000, 32000, 44100, 48000, 96000 "
                << std::endl;
      return false;
    }
    if (FIR_coeff[i].rate_ == sampling_frequency) {
      if (FIR_coeff[i].coeff_.size() == kNumberFIRTaps) {
        std::cout << "Size FIR Filter must be : " << kNumberFIRTaps
                  << std::endl;
        return false;
      }
      fir_coeff_ = FIR_coeff[i].coeff_;
      return SetFIRCoeff();
    }
  }
}
}; // namespace matrix_hal