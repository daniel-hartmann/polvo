#include "Main.h"

PolvoApplication::PolvoApplication() {}

PolvoApplication::~PolvoApplication() {}

const juce::String PolvoApplication::getApplicationName() { 
    return ProjectInfo::projectName; 
}

const juce::String PolvoApplication::getApplicationVersion() { 
    return ProjectInfo::versionString; 
}

bool PolvoApplication::moreThanOneInstanceAllowed() { 
    return true; 
}

void PolvoApplication::initialise(const juce::String& commandLine) {
    mainComponent = std::make_unique<MainComponent>();
    mainWindow.reset(new MainWindow(getApplicationName(), std::move (mainComponent)));
}

void PolvoApplication::shutdown() {
    mainWindow = nullptr;
}


void PolvoApplication::systemRequestedQuit() {
    quit();
}

void PolvoApplication::anotherInstanceStarted(const juce::String& commandLine) {
    // Handle multiple instances if necessary
}


//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (PolvoApplication)
