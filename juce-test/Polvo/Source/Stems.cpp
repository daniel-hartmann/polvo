#include "Stems.h"

Stems::Stems()
{
    // Ensure dataPath exists
    if (!dataPath.exists()) {
        dataPath.createDirectory();
    }

    // Create and configure sliders for vocals, drums, bass, and other
    setupSlider(vocalsSlider, vocalsSliderLabel, "VOCALS");
    setupSlider(drumsSlider, drumsSliderLabel, "DRUMS");
    setupSlider(bassSlider, bassSliderLabel, "BASS");
    setupSlider(pianoSlider, pianoSliderLabel, "PIANO");
    setupSlider(otherSlider, otherSliderLabel, "OTHER");


    createAndSetupButton(recordButton, recordButtonLabel, "RECORD",
    juce::Colour(0xff8b4513), [this]() { startRecording(); });
    createAndSetupButton(stopButton, stopButtonLabel, "STOP",
    juce::Colour(0xffd2b48c), [this]() { stopRecording(); });
    createAndSetupButton(playPauseButton, playPauseButtonLabel, "PLAY",
    juce::Colour(0xffd2b48c), [this]() { playPause(); });

    addAndMakeVisible(recordingsListBox);
    recordingsListBox.setModel(this);
    recordingsListBox.setColour(juce::ListBox::backgroundColourId,
                                juce::Colour(0xffe6e6e6)); // Set background color
    recordingsListBox.setColour(juce::ListBox::textColourId,
                                juce::Colour(0xff8b4513));       // Set text color
    recordingsListBox.setColour(juce::ListBox::outlineColourId,
                                juce::Colour(0xff8b4513));     // Set outline color

    addAndMakeVisible(separateButton);
    separateButton.setButtonText("SEPARATE STEMS");
    separateButton.onClick = [this]() { separateStems(); };

    addAndMakeVisible(deleteButton);
    deleteButton.setButtonText("DELETE");
    deleteButton.onClick = [this]() { deleteSelectedRecording(); };

    updateRecordingsList();
}

Stems::~Stems()
{
    stopRecording();
}

void Stems::setupSlider(juce::Slider& slider, juce::Label& label,
                        const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::left);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff8b4513));
    juce::Font boldFont(12.0f, juce::Font::bold);
    label.setFont(boldFont);

    slider.setRange(-40.0, 12.0);
    slider.setValue(10.0);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);

    addAndMakeVisible(label);
    addAndMakeVisible(slider);

    slider.setEnabled(false);
    slider.setLookAndFeel(&lookAndFeel);
}

void Stems::createAndSetupButton(juce::TextButton& button, juce::Label& label,
                                 const juce::String& labelText, const juce::Colour& buttonColor,
                                 std::function<void()> onClick)
{
    // Add button and label to component
    addAndMakeVisible(button);
    addAndMakeVisible(label);

    // Configure label for the button
    label.setColour(juce::Label::textColourId, juce::Colour(0xff8b4513));
    label.setText(labelText, juce::dontSendNotification);
    juce::Font boldFont(12.0f, juce::Font::bold);
    label.setFont(boldFont);

    label.setJustificationType(juce::Justification::centred);

    // Configure button properties
    // button.setButtonText("");
    button.setColour(juce::TextButton::buttonColourId, buttonColor);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    button.onClick = onClick;
}

void Stems::paint(juce::Graphics& g)
{
    // Implement custom painting if needed
}

void Stems::resized()
{
    const auto bounds = getLocalBounds();

    const int width = bounds.getWidth();
    const int height = bounds.getHeight();

    const int numSliders = 4;
    const int labelHeight = 14;
    const int sliderHeight = 30;
    const int buttonWidth = 75;
    const int buttonHeight = 50;
    const int auxButtonWidth = 90;
    const int auxButtonHeight = 20;
    const int buttonsMargin = 4;
    const int margin = 20;

    // Calculate dimensions for sliders and buttons
    int sliderWidth = width - 2 * margin;
    int buttonX = margin;
    int buttonY = height - buttonHeight - margin;

    int offsetY = margin;

    // Position sliders
    vocalsSliderLabel.setBounds(margin, offsetY, sliderWidth, labelHeight);
    offsetY += labelHeight;
    vocalsSlider.setBounds(margin, offsetY, sliderWidth, sliderHeight);
    offsetY += sliderHeight;

    drumsSliderLabel.setBounds(margin, offsetY, sliderWidth, labelHeight);
    offsetY += labelHeight;
    drumsSlider.setBounds(margin, offsetY, sliderWidth, sliderHeight);
    offsetY += sliderHeight;

    bassSliderLabel.setBounds(margin, offsetY, sliderWidth, labelHeight);
    offsetY += labelHeight;
    bassSlider.setBounds(margin, offsetY, sliderWidth, sliderHeight);
    offsetY += sliderHeight;

    otherSliderLabel.setBounds(margin, offsetY, sliderWidth, labelHeight);
    offsetY += labelHeight;
    otherSlider.setBounds(margin, offsetY, sliderWidth, sliderHeight);
    offsetY += sliderHeight + margin;

    int listBoxHeight = height - offsetY - buttonHeight - auxButtonHeight - 3 * margin;
    recordingsListBox.setBounds(margin, offsetY, width - 2 * margin, listBoxHeight);
    offsetY += listBoxHeight + 2;

    int listWidth = recordingsListBox.getWidth();
    int listHeight = recordingsListBox.getHeight();
    int auxButtonX = listWidth - auxButtonWidth;

    separateButton.setBounds(auxButtonX - auxButtonWidth - margin, offsetY,
                             auxButtonWidth, auxButtonHeight);
    deleteButton.setBounds(auxButtonX, offsetY, auxButtonWidth, auxButtonHeight);

    offsetY += auxButtonHeight + margin;

    // Position labels
    recordButtonLabel.setBounds(buttonX, buttonY - labelHeight, buttonWidth, labelHeight);
    stopButtonLabel.setBounds(buttonX + buttonWidth + buttonsMargin, buttonY - labelHeight,
                              buttonWidth, labelHeight);
    playPauseButtonLabel.setBounds(buttonX + 2 * (buttonWidth + buttonsMargin),
                                   buttonY - labelHeight, buttonWidth, labelHeight);

    // Position buttons
    recordButton.setBounds(buttonX, buttonY, buttonWidth, buttonHeight);
    stopButton.setBounds(buttonX + buttonWidth + buttonsMargin, buttonY, buttonWidth,
                         buttonHeight);
    playPauseButton.setBounds(buttonX + 2 * (buttonWidth + buttonsMargin), buttonY,
                              buttonWidth,
                              buttonHeight);
}


void Stems::startRecording()
{
    if (isRecording || writer != nullptr) {
        return;
    }

    // Open a WAV file for writing
    outputWavFileName = generateUniqueFileName();
    juce::File fileToWriteTo = dataPath.getChildFile(outputWavFileName);

    std::cout << "outputWavFileName: " << outputWavFileName << std::endl;

    // Overwrite file
    if (fileToWriteTo.existsAsFile()) {
        fileToWriteTo.deleteFile();
    }

    writer.reset(audioFormat.createWriterFor(new juce::FileOutputStream(fileToWriteTo),
                 48000, 1, 16, {}, 0));

    if (writer != nullptr) {
        // Register this class as an audio callback to start receiving audio
        isRecording = true;

    } else {
        std::cerr << "Error opening WAV file for writing\n";
    }
}

void Stems::stopRecording()
{
    if (!isRecording || writer == nullptr) {
        return;
    }

    // Unregister this class as an audio callback to stop receiving audio
    writer->flush(); // Flush any remaining audio data
    writer.reset();  // Release the writer
    isRecording = false;

    updateRecording(dataPath.getChildFile(outputWavFileName));

}

void Stems::callSeparator(const Recording& recording)
{
    // Call the stem separator
    // spleeter separate -p spleeter:5stems -o output {outputWavFileName}
    const std::string command = "spleeter separate -p spleeter:5stems -o output ";

    // Formulate the full command with the output file name
    std::string fullCommand = command + dataPath.getChildFile(
                                  recording.getFile().getFileName()).getFullPathName().toStdString();


    // Execute the command using system call
    int result = std::system(fullCommand.c_str());

    vocalsSlider.setEnabled(true);
    drumsSlider.setEnabled(true);
    bassSlider.setEnabled(true);
    pianoSlider.setEnabled(true);
    otherSlider.setEnabled(true);

    // Launch a new thread to execute the command asynchronously
    // std::async(std::launch::async, [fullCommand]() {
    //     int result = std::system(fullCommand.c_str());

    //     if (result == 0) {
    //         std::cout << "Stem separation launched successfully." << std::endl;

    //     } else {
    //         std::cerr << "Error launching stem separation command." << std::endl;
    //     }
    // });
}

void Stems::processAudioData(int numSamples)
{
    if (!isRecording || writer == nullptr) {
        return;
    }

    int numChannels = 1;

    // Convert int16_t buffer to AudioBuffer<float>
    juce::AudioBuffer<float> audioBuffer(numChannels, numSamples); // Stereo audio buffer

    for (int channel = 0; channel < numChannels; ++channel) {
        for (int sample = 0; sample < numSamples; ++sample) {
            audioBuffer.setSample(channel, sample, static_cast<float>(buffer[sample]) / 32768.0f);
        }
    }

    writer->writeFromAudioSampleBuffer(audioBuffer, 0, numSamples);
}

void Stems::playPause()
{
    // Toggle play/pause state
}

void Stems::sliderValueChanged(juce::Slider* slider)
{

}

int Stems::getNumRows()
{
    return recordings.size();
}

void Stems::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height,
                             bool rowIsSelected)
{
    if (rowNumber < recordings.size()) {
        const juce::File& file = recordings[rowNumber]->getFile();
        juce::Colour textColour = juce::Colour(0xff8b4513);
        juce::Colour backgroundColour = juce::Colour(0xffe6e6e6);

        if (rowIsSelected) {
            // Customize the colors for the selected item
            textColour = juce::Colours::white;
            backgroundColour = juce::Colour(0xff8b4513);
        }

        g.fillAll(backgroundColour); // Fill the background
        g.setColour(textColour);     // Set the text color

        juce::Font boldFont(10.0f, juce::Font::bold);
        g.setFont(boldFont);

        // Draw the file name centered in the item bounds
        g.drawText(file.getFileName(), 5, 0, width - 10, height,
                   juce::Justification::centredLeft, true);
    }
}


void Stems::updateRecordingsList()
{
    // Clear existing recordings
    recordings.clear();

    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(dataPath, false,
            "*.wav", juce::File::findFiles)) {
        const juce::File file(entry.getFile());
        recordings.push_back(std::make_unique<Recording>(file));
    }

    recordingsListBox.updateContent();
}


void Stems::updateRecording(const juce::File& file)
{
    auto it = std::find_if(recordings.begin(), recordings.end(),
    [&](const std::unique_ptr<Recording>& rec) {
        return rec->getFile() == file;
    });

    if (it != recordings.end()) {
        // Update the file associated with the recording
        (*it)->setFile(file);

    } else {
        // Add new recording to the collection
        recordings.push_back(std::make_unique<Recording>(file));
    }

    repaint();
}

void Stems::separateStems()
{
    // Perform action on the selected recording
    int selectedRow = recordingsListBox.getSelectedRow();

    if (selectedRow >= 0 && selectedRow < recordings.size()) {
        const juce::File& selectedFile = recordings[selectedRow]->getFile();

        // Call the stem separator
        callSeparator(*recordings[selectedRow]);
    }
}

void Stems::deleteSelectedRecording()
{
    // Delete the selected recording
    int selectedRow = recordingsListBox.getSelectedRow();

    if (selectedRow >= 0 && selectedRow < recordings.size()) {
        recordings.erase(recordings.begin() + selectedRow);
        recordingsListBox.updateContent();
    }
}


std::string Stems::generateUniqueFileName()
{
    juce::String timeStamp = juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S");
    juce::String randomPart = juce::String::toHexString(
                                  juce::Random::getSystemRandom().nextInt());
    return "output_" + timeStamp.toStdString() + "_" + randomPart.toStdString() + ".wav";
}
