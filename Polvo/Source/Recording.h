/*
  ==============================================================================

    Recording.h
    Created: 4 May 2024 12:24:26am
    Author:  daniel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Recording
{
public:
    Recording(const juce::File& file);
    Recording(const juce::DirectoryEntry& entry);
    const juce::String& getFileName() const;
    void setFileName(const juce::String& newName);
    void setFile(const juce::File& newName);
    const juce::File& getFile() const;

private:
    juce::File file;
    juce::String fileName;
};
