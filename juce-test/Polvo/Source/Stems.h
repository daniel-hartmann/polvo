#pragma once

// #include <future>
#include <JuceHeader.h>
#include <pulse/pulseaudio.h>
#include <vector>
#include <algorithm>
#include "Config.h"
#include "PolvoTheme.h"
#include "Recording.h"
#include "Mixer.h"
#include "Waveform.h"

class Stems : public juce::Component, public juce::Slider::Listener,
    public juce::ListBoxModel

{
public:
    Stems();
    ~Stems();

    int16_t buffer[BUFFER_SIZE];

    void paint(juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

    void setWaveform(Waveform& waveformRef);

    void processAudioData(int numSamples);

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
                          int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;


private:
    // dataPath = './data'
    const juce::File dataPath =
        juce::File::getCurrentWorkingDirectory().getChildFile("data");
    std::string outputWavFileName;
    std::vector<std::unique_ptr<Recording>> recordings;
    std::vector<std::unique_ptr<juce::AudioTransportSource>> transportSources;
    std::vector<juce::Slider*> sliders;

    Waveform* waveform = nullptr;

    bool isRecording = false;

    PolvoLookAndFeel lookAndFeel;

    juce::Label statusBar;

    // For playing the wav file
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer sourcePlayer;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    std::unique_ptr<juce::AudioTransportSource> transportSource;
    std::unique_ptr<juce::AudioFormatReader> reader;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    // std::unique_ptr<juce::MixerAudioSource> mixer;
    std::unique_ptr<Mixer> mixer;

    // For recording a wav file
    juce::WavAudioFormat audioFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    juce::Slider vocalsSlider;
    juce::Slider drumsSlider;
    juce::Slider bassSlider;
    juce::Slider pianoSlider;
    juce::Slider otherSlider;

    juce::Label vocalsSliderLabel;
    juce::Label drumsSliderLabel;
    juce::Label bassSliderLabel;
    juce::Label pianoSliderLabel;
    juce::Label otherSliderLabel;

    struct TrackData {
        const char* filename;
        std::function<double()> sliderValueGetter;
    };

    juce::TextButton recordButton;
    juce::TextButton stopButton;
    juce::TextButton playPauseButton;

    juce::Label recordButtonLabel;
    juce::Label stopButtonLabel;
    juce::Label playPauseButtonLabel;

    juce::ListBox recordingsListBox;
    juce::TextButton separateButton;
    juce::TextButton deleteButton;

    // juce::Colour recordingsListBGColor = juce::Colour(0xff8b4513);
    juce::Colour recordingsListBGColor = PolvoColor::darkBlue;

    void setupSlider(juce::Slider & slider, juce::Label& label, const juce::String & text);
    void createAndSetupButton(juce::TextButton& button, juce::Label& label,
                              const juce::String& labelText, const juce::Colour& buttonColor,
                              std::function<void()> onClick);

    // Recording
    void startRecording();
    void stopRecording();
    void actionStopRecording();

    // Play/Pause and Mixer
    void playPause();
    void playOriginalFile(const juce::File& file);
    void playStemsTogether(const juce::File& file);
    // void loadAndAddStemToMixer(const juce::File& stemFile);
    void stopAllTransportSources();

    // Call stems separator command
    void callSeparator(const Recording& recording);

    // Recordings list and its actions
    void updateRecordingsList();
    void updateRecording(const juce::File& file);
    void separateStems();
    void deleteSelectedRecording();

    // Utils
    std::string generateUniqueFileName();
    juce::File getStemsFolder(const juce::File& selectedFile);
    void setStatus(std::string status);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stems)
};
