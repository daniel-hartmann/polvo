#pragma once

#include <JuceHeader.h>
#include "MainWindow.h"

using namespace juce;

class PolvoApplication  : public juce::JUCEApplication
{
public:
    PolvoApplication();
    ~PolvoApplication() override;

    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;

    void initialise (const juce::String& commandLine) override;
    void shutdown() override;
    void systemRequestedQuit() override;
    void anotherInstanceStarted (const juce::String& commandLine) override;

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<MainComponent> mainComponent;
};
