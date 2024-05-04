#pragma once

// #include <future>
#include <JuceHeader.h>
#include <pulse/pulseaudio.h>
#include <vector>
#include <algorithm>
#include "PolvoLookAndFeel.h"
#include "Recording.h"

#define BUFFER_SIZE 1024


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

    void processAudioData(int numSamples);

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
                          int width, int height, bool rowIsSelected) override;


private:
    // dataPath = './data'
    const juce::File dataPath =
        juce::File::getCurrentWorkingDirectory().getChildFile("data");
    std::string outputWavFileName;
    std::vector<std::unique_ptr<Recording>> recordings;

    bool isRecording = false;

    PolvoLookAndFeel lookAndFeel;

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

    juce::TextButton recordButton;
    juce::TextButton stopButton;
    juce::TextButton playPauseButton;

    juce::Label recordButtonLabel;
    juce::Label stopButtonLabel;
    juce::Label playPauseButtonLabel;

    juce::ListBox recordingsListBox;
    juce::TextButton separateButton;
    juce::TextButton deleteButton;

    void setupSlider(juce::Slider & slider, juce::Label& label, const juce::String & text);
    void createAndSetupButton(juce::TextButton& button, juce::Label& label,
                              const juce::String& labelText, const juce::Colour& buttonColor,
                              std::function<void()> onClick);

    std::string generateUniqueFileName();
    void startRecording();
    void stopRecording();
    void playPause();
    void callSeparator(const Recording& recording);

    void updateRecordingsList();
    void updateRecording(const juce::File& file);
    void separateStems();
    void deleteSelectedRecording();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stems)
};
