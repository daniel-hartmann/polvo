#include "Stems.h"

Stems::Stems() : deviceManager(), transportSource(new juce::AudioTransportSource())
{
    // Ensure dataPath exists
    if (!dataPath.exists()) {
        dataPath.createDirectory();
    }

    // mixer.reset(new juce::MixerAudioSource());
    mixer.reset(new Mixer(&deviceManager));
    // mixer = std::make_unique<Mixer>();

    deviceManager.initialiseWithDefaultDevices(0, 2);
    sourcePlayer.setSource(transportSource.get());
    deviceManager.addAudioCallback(&sourcePlayer);
    formatManager.registerBasicFormats();

    // Create and configure sliders for vocals, drums, bass, and other
    setupSlider(vocalsSlider, vocalsSliderLabel, "VOCALS");
    setupSlider(drumsSlider, drumsSliderLabel, "DRUMS");
    setupSlider(bassSlider, bassSliderLabel, "BASS");
    setupSlider(pianoSlider, pianoSliderLabel, "PIANO");
    setupSlider(otherSlider, otherSliderLabel, "OTHER");


    createAndSetupButton(recordButton, recordButtonLabel, "RECORD",
    PolvoColor::brown, [this]() { startRecording(); });
    createAndSetupButton(stopButton, stopButtonLabel, "STOP",
    PolvoColor::lightYellow, [this]() { actionStopRecording(); });
    createAndSetupButton(playPauseButton, playPauseButtonLabel, "PLAY",
    PolvoColor::lightYellow, [this]() { playPause(); });

    addAndMakeVisible(recordingsListBox);
    recordingsListBox.setModel(this);
    recordingsListBox.setColour(juce::ListBox::backgroundColourId, recordingsListBGColor);

    addAndMakeVisible(separateButton);
    separateButton.setButtonText("SEPARATE STEMS");
    separateButton.onClick = [this]() { separateStems(); };
    separateButton.setColour(juce::TextButton::buttonColourId, PolvoColor::lightYellow);
    separateButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff8b4513));


    addAndMakeVisible(deleteButton);
    deleteButton.setButtonText("DELETE");
    deleteButton.onClick = [this]() { deleteSelectedRecording(); };
    deleteButton.setColour(juce::TextButton::buttonColourId, PolvoColor::lightYellow);
    deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff8b4513));

    recordingsListBox.setLookAndFeel(&lookAndFeel);
    separateButton.setLookAndFeel(&lookAndFeel);
    deleteButton.setLookAndFeel(&lookAndFeel);

    // Create and configure status bar
    addAndMakeVisible(statusBar);
    statusBar.setColour(juce::Label::textColourId, PolvoColor::lightYellow);
    statusBar.setColour(juce::Label::outlineColourId, PolvoColor::lessDark);
    statusBar.setText("Ready", juce::dontSendNotification);
    statusBar.setJustificationType(juce::Justification::centredLeft);
    statusBar.setFont(12.0f);
    statusBar.setBorderSize(juce::BorderSize<int>(1));

    updateRecordingsList();
}

Stems::~Stems()
{
    stopRecording();
    setLookAndFeel(nullptr);
    stopAllTransportSources();
}

void Stems::setupSlider(juce::Slider & slider, juce::Label & label,
                        const juce::String & text)
{
    sliders.push_back(&slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::left);
    label.setColour(juce::Label::textColourId, PolvoColor::orange);
    juce::Font boldFont(12.0f, juce::Font::bold);
    label.setFont(boldFont);

    slider.setRange(0.0, 1.0);
    slider.setValue(0.8);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);

    addAndMakeVisible(label);
    addAndMakeVisible(slider);

    slider.setEnabled(false);
    slider.setLookAndFeel(&lookAndFeel);

    slider.addListener(this);
}

void Stems::createAndSetupButton(juce::TextButton & button, juce::Label & label,
                                 const juce::String & labelText, const juce::Colour & buttonColor,
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
    button.setColour(juce::TextButton::buttonColourId, buttonColor);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    button.onClick = onClick;
}

void Stems::paint(juce::Graphics & g)
{
    // Implement custom painting if needed
}

void Stems::resized()
{
    const auto bounds = getLocalBounds();

    const int width = bounds.getWidth();
    const int height = bounds.getHeight();

    const int numSliders = 4;
    const int labelHeight = 10;
    const int sliderHeight = 30;
    const int buttonWidth = 75;
    const int buttonHeight = 50;


    const int auxButtonHeight = 20;
    const int separateButtonWidth = 100;
    const int deleteButtonWidth = 70;

    const int statusBarHeight = 20;

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

    pianoSliderLabel.setBounds(margin, offsetY, sliderWidth, labelHeight);
    offsetY += labelHeight;
    pianoSlider.setBounds(margin, offsetY, sliderWidth, sliderHeight);
    offsetY += sliderHeight;

    otherSliderLabel.setBounds(margin, offsetY, sliderWidth, labelHeight);
    offsetY += labelHeight;
    otherSlider.setBounds(margin, offsetY, sliderWidth, sliderHeight);
    offsetY += sliderHeight + 10;

    int listBoxHeight = height - offsetY - buttonHeight - auxButtonHeight - statusBarHeight
                        - 2 * margin;
    recordingsListBox.setBounds(margin, offsetY, width - 2 * margin, listBoxHeight);
    offsetY += listBoxHeight + 2;

    int listWidth = recordingsListBox.getWidth();
    int listHeight = recordingsListBox.getHeight();

    // Don't know why I had to sum the margin here
    int auxButtonX = listWidth - deleteButtonWidth + margin;

    separateButton.setBounds(auxButtonX - separateButtonWidth - buttonsMargin, offsetY,
                             separateButtonWidth, auxButtonHeight);
    deleteButton.setBounds(auxButtonX, offsetY, deleteButtonWidth, auxButtonHeight);

    offsetY += auxButtonHeight + margin;

    statusBar.setBounds(margin, height - buttonHeight - labelHeight - 2 * margin,
                        width - 2 * margin,
                        statusBarHeight);

    int bwidth = (width - 2 * margin - 2 * buttonsMargin) / 3;

    // Position labels
    recordButtonLabel.setBounds(buttonX, buttonY - labelHeight, bwidth, labelHeight);
    stopButtonLabel.setBounds(buttonX + bwidth + buttonsMargin, buttonY - labelHeight,
                              bwidth, labelHeight);
    playPauseButtonLabel.setBounds(buttonX + 2 * (bwidth + buttonsMargin),
                                   buttonY - labelHeight, bwidth, labelHeight);

    // Position buttons
    recordButton.setBounds(buttonX, buttonY, bwidth, buttonHeight);
    stopButton.setBounds(buttonX + bwidth + buttonsMargin, buttonY, bwidth,
                         buttonHeight);
    playPauseButton.setBounds(buttonX + 2 * (bwidth + buttonsMargin), buttonY,
                              bwidth,
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
}

void Stems::actionStopRecording()
{
    stopRecording();
    updateRecording(dataPath.getChildFile(outputWavFileName));
    updateRecordingsList();
}

void Stems::callSeparator(const Recording & recording)
{
    // Call the stem separator
    // spleeter separate -p spleeter:5stems -o output {outputWavFileName}
    const std::string command = "spleeter separate -p spleeter:5stems -o ";

    // Formulate the full command with the output file name
    std::string fullCommand = command + dataPath.getFullPathName().toStdString() + " " +
                              dataPath.getChildFile(
                                  recording.getFile().getFileName()).getFullPathName().toStdString();

    setStatus("Separating stems...");

    // Execute the command using system call
    int result = std::system(fullCommand.c_str());

    setStatus("Separating stems... Finished.");

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
    int selectedRow = recordingsListBox.getSelectedRow();

    if (selectedRow >= 0 && selectedRow < recordings.size()) {
        const juce::File& selectedFile = recordings[selectedRow]->getFile();

        if (selectedFile.existsAsFile()) {
            juce::File stemsFolder = getStemsFolder(selectedFile);

            if (stemsFolder.exists() && stemsFolder.isDirectory()) {
                playStemsTogether(selectedFile);

            } else {
                playOriginalFile(selectedFile);
            }

        } else {
            std::cerr << "Error: Selected file does not exist." << std::endl;
        }

    } else {
        std::cerr << "Error: Invalid selected row." << std::endl;
    }
}

void Stems::playOriginalFile(const juce::File& file)
{
    stopAllTransportSources();

    juce::AudioDeviceManager::AudioDeviceSetup deviceSetup =
        juce::AudioDeviceManager::AudioDeviceSetup();
    deviceSetup.sampleRate = 48000;
    deviceManager.initialise(2, 2, 0, true, {}, &deviceSetup);

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr) {
        if (transportSource->isPlaying()) {
            transportSource->stop();
            transportSource->setSource(nullptr);

            setStatus("Paused " + file.getFileName().toStdString());

        } else {
            // Create AudioFormatReaderSource and set as transportSource's source
            juce::AudioFormatReaderSource* readerSource = new juce::AudioFormatReaderSource(
                reader.release(), false);

            transportSource->setSource(readerSource);
            transportSource->start();

            setStatus("Playing " + file.getFileName().toStdString());
        }

    } else {
        std::cerr << "Error: Failed to create AudioFormatReader for playback." << std::endl;
    }
}

void Stems::playStemsTogether(const juce::File& file)
{
    std::cout << "playStemsTogether" << std::endl;

    std::cout << "stopAllTransportSources...\t";
    stopAllTransportSources();
    std::cout << "OK" << std::endl;

    // Initialize audio device manager if not already initialized
    std::cout << "initialising device...\t";
    juce::AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceSetup.sampleRate = 44100;
    deviceManager.initialise(2, 2, 0, true, {}, &deviceSetup);
    std::cout << "OK" << std::endl;

    // Create a new mixer for managing multiple audio sources
    // std::cout << "reseting mixer...\t";
    // mixer.reset(new Mixer(&deviceManager));
    // std::cout << "OK" << std::endl;

    // Load and add stems to the mixer
    int trackIndex = 0;

    std::vector<TrackData> tracks_data = {
        {"vocals.wav", [&]() { return vocalsSlider.getValue(); }},
        {"drums.wav", [&]() { return drumsSlider.getValue(); }},
        {"bass.wav", [&]() { return bassSlider.getValue(); }},
        {"piano.wav", [&]() { return pianoSlider.getValue(); }},
        {"other.wav", [&]() { return otherSlider.getValue(); }},
        // Add more tracks here...
    };

    std::cout << "Iterating over stems..." << std::endl;

    for (const auto& track : tracks_data) {
        const char* stemFilename = track.filename;
        double sliderValue = track.sliderValueGetter();

        std::cout << "trackIndex: " << trackIndex << std::endl;

        std::cout << "getting stem file...\t";
        juce::File stemFile = getStemsFolder(file).getChildFile(stemFilename);
        std::cout << "OK" << std::endl;

        if (stemFile.existsAsFile()) {

            std::cout << "initialising mixer for " << stemFilename << "..." << std::endl;
            mixer->loadAudioFile(trackIndex, stemFile, sliderValue);
            std::cout << "initialising mixer: OK" << std::endl;

        } else {
            std::cerr << "Warning: Stem file '" << stemFilename << "' not found." << std::endl;
        }

        trackIndex += 1;
    }

    std::cout << "mixer->playPause()" << std::endl << std::endl;
    mixer->playPause();
}

void Stems::stopAllTransportSources()
{
    // Reset mixer
    // mixer->removeAllInputs();

    // Stop and release all transport sources
    for (auto& source : transportSources) {
        if (source == nullptr) { continue; }

        if (source->isPlaying()) {
            source->stop();
        }

        source->setSource(nullptr); // Release the source
    }

    transportSources.clear();
}

void Stems::sliderValueChanged(juce::Slider* slider)
{
    std::cout << "Stems::sliderValueChanged" << std::endl;

    if (slider == &vocalsSlider) {
        mixer->setGain(0, vocalsSlider.getValue());

    } else if (slider == &drumsSlider) {
        mixer->setGain(1, drumsSlider.getValue());

    } else if (slider == &bassSlider) {
        mixer->setGain(2, bassSlider.getValue());

    } else if (slider == &pianoSlider) {
        mixer->setGain(3, pianoSlider.getValue());

    } else if (slider == &otherSlider) {
        mixer->setGain(4, otherSlider.getValue());
    }
}

int Stems::getNumRows()
{
    return recordings.size();
}

void Stems::paintListBoxItem(int rowNumber, juce::Graphics & g, int width, int height,
                             bool rowIsSelected)
{
    if (rowNumber < recordings.size()) {
        const juce::File& file = recordings[rowNumber]->getFile();
        juce::Colour textColour = juce::Colour(0xffe6e6e6);
        juce::Colour backgroundColour = recordingsListBGColor;

        if (rowIsSelected) {
            // Customize the colors for the selected item
            textColour = recordingsListBGColor;
            backgroundColour = juce::Colour(0xffe6e6e6);
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

void Stems::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < recordings.size()) {
        const juce::File& selectedFile = recordings[row]->getFile();
        juce::String baseName = selectedFile.getFileNameWithoutExtension();
        juce::File stemsFolder = dataPath.getChildFile(baseName);

        if (stemsFolder.exists() && stemsFolder.isDirectory()) {
            // Stems folder exists for this file
            std::cout << "Stems folder found for file: " << selectedFile.getFileName() << std::endl;

            vocalsSlider.setEnabled(true);
            drumsSlider.setEnabled(true);
            bassSlider.setEnabled(true);
            pianoSlider.setEnabled(true);
            otherSlider.setEnabled(true);

        } else {
            // Stems folder does not exist for this file
            std::cout << "Stems folder not found for file: " << selectedFile.getFileName() <<
                      std::endl;

            vocalsSlider.setEnabled(false);
            drumsSlider.setEnabled(false);
            bassSlider.setEnabled(false);
            pianoSlider.setEnabled(false);
            otherSlider.setEnabled(false);

            // Get waveform from MainComponent using parent stuff
            waveform->setFile(selectedFile);
            waveform->setDisplayMode(Waveform::File);


        }

    } else {
        std::cerr << "Invalid selected row in ListBox" << std::endl;
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


void Stems::updateRecording(const juce::File & file)
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

        // Extract the base name (file name without extension)
        juce::String baseName = selectedFile.getFileNameWithoutExtension();

        // Specify the folder where stems are expected
        juce::File stemsFolder = dataPath.getChildFile(baseName);

        // Check if the stems folder exists
        // TODO: check if contains files
        if (stemsFolder.exists() && stemsFolder.isDirectory()) {
            // Already exists, do nothing.
        } else {
            callSeparator(*recordings[selectedRow]);
        }

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

void Stems::setStatus(std::string status)
{
    statusBar.setText(status, juce::dontSendNotification);
    statusBar.repaint();
}


std::string Stems::generateUniqueFileName()
{
    juce::String timeStamp = juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S");
    juce::String randomPart = juce::String::toHexString(
                                  juce::Random::getSystemRandom().nextInt());
    return "output_" + timeStamp.toStdString() + "_" + randomPart.toStdString() + ".wav";
}

juce::File Stems::getStemsFolder(const juce::File& selectedFile)
{
    juce::String baseName = selectedFile.getFileNameWithoutExtension();
    return dataPath.getChildFile(baseName);
}

void Stems::setWaveform(Waveform& waveformRef)
{
    waveform = &waveformRef;
}
